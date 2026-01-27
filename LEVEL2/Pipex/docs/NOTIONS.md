# PIPEX: Master the Pipe

> "Connect programs like a garden hose." — Douglas McIlroy, 1964

**Pipex** is your first deep dive into Unix System Programming (Process API). It challenges you to mimic the shell's pipe mechanism (`|`), connecting the output of one process to the input of another via the kernel.

---

## I. The Mission

### Goal
Replicate the shell command:
```bash
< infile cmd1 | cmd2 > outfile
```
Run as:
```bash
./pipex infile "cmd1" "cmd2" outfile
```

### 1. Mandatory Requirements
- **Arguments:** 4 inputs (`infile`, `cmd1`, `cmd2`, `outfile`).
- **Data Flow:** Read `infile` → `cmd1` → Pipe → `cmd2` → `outfile`.
- **Environment:** Must use `PATH` to find commands (e.g., convert `ls` to `/usr/bin/ls`).
- **Error Handling:** Must behave like the shell (file permission errors, command not found, etc.).
- **Memory:** Zero leaks allowed.

### 2. Bonus Requirements
- **Multi-Pipe:** Handle `N` commands.
  ```bash
  # < infile cmd1 | cmd2 | cmd3 ... | cmdN > outfile
  ./pipex infile cmd1 cmd2 cmd3 ... cmdN outfile
  ```
- **Here_doc:** Support `<<` redirection.
  ```bash
  # cmd1 << LIMITER | cmd2 >> outfile
  ./pipex here_doc LIMITER cmd1 cmd2 outfile
  ```

---

## II. Core Concepts

### 1. Processes & Threads
| Feature | Process (Fork) | Thread |
|:--- |:---|:---|
| **Memory** | **Isolated.** Copy of parent. | **Shared.** Same heap/variables. |
| **Stability** | Child crash doesn't kill parent. | One thread crash kills process. |
| **Communication**| IPC (Pipes, Signals). | Mutexes, Shared Memory. |
| **Usage in Pipex**| **Required**. `execve` replaces memory, so we need a throwaway process (child). | **Useless**. `execve` would kill the whole program. |

### 2. File Descriptors (FDs)
Integers connecting user code to kernel I/O resources.
- `0`: STDIN (Standard Input)
- `1`: STDOUT (Standard Output)
- `2`: STDERR (Standard Error)

**Key Function: `dup2(oldfd, newfd)`**
- Redirects `newfd` to point to the same resource as `oldfd`.
- **Example:** `dup2(pipe_write_end, STDOUT_FILENO)` makes all `printf` calls write into the pipe.

### 3. The Pipe (`pipe()`)
A unidirectional channel in Kernel RAM (not disk).
- **Fd[0]**: Read end (Result).
- **Fd[1]**: Write end (Input).
- **Buffer**: Ring buffer (usually 64KB). Writes block if full; reads block if empty.

---

## III. The Mental Model (Analogies)

If the code is confusing, imagine a **Restaurant Kitchen**.

**1. File Descriptors are "Service Trays"**
Every worker has a desk with numbered slots.
- **Slot 0 (STDIN):** Where raw ingredients arrive.
- **Slot 1 (STDOUT):** Where finished dishes go.
- **Slot 2 (STDERR):** Where burnt food happens.
- **Rules:** By default, Slot 1 connects to the Dining Room (Terminal).

**2. The Pipe is a "Chute"**
A one-way metal slide between two stations.
- You cannot see inside it.
- You shove items in the top (Write End), and they slide to the bottom (Read End).

**3. Fork is "Cloning"**
You (The Manager) snap your fingers.
- **Poof!** An exact physical copy of you appears.
- **The difference:** You know who the clone is (PID). The clone looks at their nametag and sees "0" (Newbie).

**4. Dup2 is "Swapping Labels"**
The Clone needs to slice potatoes (`cmd1`) into the Chute, not the Dining Room.
- **Problem:** The Potato Slicer (`ls`) is stupid; it *always* puts output in **Slot 1**.
- **The Hack (`dup2`):** You unplug the Dining Room from Slot 1 and plug in the **Chute Entrance** instead.
- Now, when the worker puts food in Slot 1, it slides down the chute automatically.

**5. Execve is "Hypnosis / Brain Transplant"**
The Clone starts with your brain (your C code).
- You call `execve`.
- The Clone **forgets everything**. Its memory is wiped.
- It downloads the brain of a "Potato Slicer" (`/bin/ls`).
- It wakes up and immediately starts slicing, unaware it was ever a clone.

---

## IV. Deep Dive: Under the Hood

### 1. The Kernel Ring Buffer
Data moves from User Space `Process A` → Kernel Space `Buffer` → User Space `Process B`.

```text
    USER SPACE (Unsafe)                 |          KERNEL SPACE (Protected)
                                        |
+------------------------------+        |       +-----------------------------------+
| PROCESS A (Writer)           |        |       |        THE PIPE BUFFER            |
| write(pipe_fd[1], buf, 5); --+--------+-----> |     WRITE_PTR (Head)              |
+------------------------------+        |       |         |                         |
                                        |       |         v                         |
                                        |       |   [ ... | H | E | L | L | O ]     |
                                        |       |   [ ... |   |   |   |   |   ]     |
                                        |       |             ^                     |
+------------------------------+        |       |             |                     |
| PROCESS B (Reader)           |        |       |     READ_PTR (Tail)               |
| read(pipe_fd[0], rec, 5); <--+--------+-----+ |                                   |
+------------------------------+        |       +-----------------------------------+
```

### 2. Execution Flow Schema (2 Commands)

```text
      TIME       PARENT PROCESS            CHILD 1 (Writer)          CHILD 2 (Reader)
       |               |                          |                         |
       |         pipe() created                   |                         |
       |         fork() -----------------------> START                      |
       |               |                          |                         |
       |         fork() -------------------------------------------------> START
       |               |                          |                         |
       |         close(pipe_fds)                  |                         |
       |         waitpid(pid1) ...waiting...    dup2(pipe, STDOUT)      dup2(pipe, STDIN)
       |               |                        execve(cmd1)            execve(cmd2)
       |               |                          |                         |
       |               |                      WRITES DATA ------------>  READS DATA
       |               |                          |                         |
       |               |                         DIES                       |
       V         [parent unblocks] <--------- (Exit Status)                 |
                 waitpid(pid2) ...waiting...                                |
                       |                                                   DIES
                 [parent unblocks] <--------------------------------- (Exit Status)
                 EXIT SUCCESS
```

### 3. The Deadlock Trap
**Scenario:** Parent waits (`waitpid`) for Child 1 before Child 1 finishes.
**The deadlock:**
1. Child 1 fills the pipe buffer (64KB).
2. Child 1 **blocks** (sleeps) waiting for space to be freed.
3. Parent is **blocked** waiting for Child 1 to exit.
4. Child 2 (Reader) is never forked or never scheduled to read.
5. **Result:** Universe freezes.
**Fix:** Fork ALL children, close unused FDs, THEN wait for all.

---

## V. Extended Concepts

### 1. The `fork()` Return Value Mystery
When `fork()` is called, the process splits. The return value tells you **who you are**:
- **Parent Process:** Receives the **PID of the Child** (e.g., `12345`). It uses this ID to control or wait for the child later.
- **Child Process:** Receives **`0`**. This is not its PID, but a flag saying "You are the newly created one".

```c
pid_t pid = fork();
if (pid == 0)
    // I am the Child
else
    // I am the Parent, and 'pid' holds the Child's ID
```

### 2. Memory & Function Storage
- **After Fork:** The child shares the parent's code (Read-Only Text Segment). It has access to all functions (`ft_strlen`, `main`) because it's a clone.
- **After `execve` (The "Brain Transplant"):** `execve` wipes the child's memory. Your functions are gone. They are replaced by the code of the new command (e.g., `ls`).
- **Function Pointers:** When you pass a function name (like in MinilibX hooks), you are passing the memory address of that code block.

### 3. Detailed `waitpid` Mechanics
`waitpid` is the Parent's tools to synchronize with Children.
- **Prototype:** `waitpid(pid, &status, options)`
- **`status`:** An integer packing multiple details (Exit Code + Termination Signal).
  - Use macros to decode: `WIFEXITED(status)`, `WEXITSTATUS(status)`.
- **Why it hangs:** It is a **blocking** call. The parent sleeps at that line until the specific child changes state (dies).

### 4. Finding Commands (`PATH`)
Your program doesn't know where `ls` is.
1.  **Read `envp`:** Look for the string starting with `PATH=`.
2.  **Split:** Break it into folders (e.g., `/bin`, `/usr/bin`).
3.  **Construct:** Join folder + `/` + command (e.g., `/bin/ls`).
4.  **Verify:** Use `access(fullpath, X_OK)` to check if it's executable.

### 5. `dup2` and `STDOUT_FILENO`
- **Misconception:** You cannot change the value of `STDOUT_FILENO` (it's always `1`).
- **Reality:** You change what Index `1` **points to**.
- **Analogy:** `dup2` is like changing the destination of Speed Dial Button #1. The button is still labeled "1", but now it calls your file instead of the screen.

---

## VI. Implementation Strategy

### Step 1: Parsing
- Split `PATH` variable (`ft_split`).
- Append command names to paths (`/bin` + `/` + `ls`).
- Check access (`access()`).

### Step 2: The Logic Loop (Bonus/General)
For `n` commands, you need `n-1` pipes or a rolling pipe strategy.

**Rolling Pipe Strategy:**
Iteration `i`:
1. `pipe(new_fd)`.
2. `fork()`.
3. **Child:**
   - Input: Read from `old_fd` (previous command's output).
   - Output: Write to `new_fd[1]` (current pipe).
4. **Parent:**
   - Close `old_fd`.
   - Close `new_fd[1]`.
   - Save `new_fd[0]` as `old_fd` for next iteration.

### Step 3: Cleanup
- **Close everything.** If a write-end stays open, `read()` never returns 0 (EOF), and the reader hangs forever.
- **Wait.** Use `waitpid(-1, &status, 0)` in a loop to reap all zombies.

---

## VII. Common Pitfalls

1. **Zombie Processes:** Failing to `waitpid` leaves dead entries in the process table.
2. **Double Free:** Freeing the `split` array incorrectly.
3. **Leaked FDs:** Not closing pipe ends in Parent causes hangs.
4. **Execve Fail:** Ignoring that `execve` might fail (path wrong). Always add an `exit(ERROR)` after `execve`.
5. **Relative Paths:** `execve` needs absolute paths (`/bin/ls`), not just `ls`.

---

## Appendix: Additional Resources

To truly master these concepts, consult the following high-quality sources:

1.  **OSTEP (Operating Systems: Three Easy Pieces)**
    *   *Chapters:* Process API (fork/exec), Inter-Process Communication.
    *   *Why:* The gold standard for OS theory. Clear, conversational, and deep.
    *   *Link:* [https://pages.cs.wisc.edu/~remzi/OSTEP/](https://pages.cs.wisc.edu/~remzi/OSTEP/)

2.  **The Linux Programming Interface (TLPI) - Michael Kerrisk**
    *   *Chapters:* Pipes and FIFOs, Process Creation.
    *   *Why:* The Bible of Linux API. Written by the maintainer of the man-pages.
    *   *Note:* Dense but definitive.

3.  **Code Vault (YouTube Channel)**
    *   *Playlist:* Unix Processes in C.
    *   *Why:* Excellent short visual explanations of `pipe`, `fork`, `exec`, and `dup2` specifically for C programmers.
    *   *Link:* [Code Vault - Unix Pipes](https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9UI90nDw663E9)

4.  **Beej's Guide to Unix IPC**
    *   *Why:* A classic, approachable guide specifically focused on Inter-Process Communication.
    *   *Link:* [https://beej.us/guide/bgipc/](https://beej.us/guide/bgipc/)

---

## VII. Bonus: The `here_doc` (Here Document)

### 1. What is it?
A **Here Document** (here_doc) is a way to pass a multi-line string directly into a command's standard input (Stdin) from the shell, without creating a separate file.

In standard shells, it uses the `<<` operator followed by a **LIMITER** keyword.

### 2. The Logic (Shell vs Pipex)
**Bash Case:**
```bash
# cat reads from stdin until it sees "EOF"
cat << EOF | grep "hello" >> outfile
hello world
this is a test
EOF
```
*   You type lines.
*   Once you type `EOF` on a new line, the shell stops reading.
*   It sends everything you just typed into `cat`.
*   `cat` passes it to `grep`.

**Pipex Case:**
The subject asks you to replicate:
```bash
./pipex here_doc LIMITER cmd1 cmd2 outfile
```
This should behave exactly like:
```bash
cmd1 << LIMITER | cmd2 >> outfile
```

### 3. Usage & Implementation Strategy
Unlike the mandatory part where input comes from a file (`infile`), in `here_doc`, input comes from **User Input (Stdin)**.

**Steps:**
1.  **Detect Mode:** Check if `argv[1]` is `"here_doc"`.
2.  **Create a Temporary Pipe:** You need a place to store what the user types before feeding it to `cmd1`. A `pipe()` is perfect for this.
3.  **Read Loop:**
    *   Read from Standard Input (File Descriptor 0) line by line (hello `get_next_line`!).
    *   Compare the line with `LIMITER` (plus `\n`).
    *   If it matches: **Stop reading**.
    *   If no match: **Write** the line into the write-end of your temporary pipe.
4.  **Execute:**
    *   For `cmd1`: Set its input (Stdin) to the *read-end* of that temporary pipe.
    *   For `outfiles`: Note that `here_doc` implies append mode (`>>`), so open your outfile with `O_WRONLY | O_CREAT | O_APPEND`.

### 4. Difficulties & Traps
1.  **The Limiter Newline:** `get_next_line` usually keeps the `\n`. Your limiter command argument (`argv[2]`) won't have it. `ft_strncmp("EOF", "EOF\n", n)` might trick you. Make sure you compare exactly.
2.  **Unlink/Cleanup:** You don't necessarily need a physical temporary file (like `.tmp_heredoc`), but if you do create one, ensure you `unlink` it before exiting. Using a `pipe` buffer in memory is cleaner and faster.
3.  **Hanging Processes:** If you don't close the write-end of your special input pipe after writing the user input, `cmd1` will wait forever for more data (EOF never comes).
4.  **Prompt:** Bash prints `> ` or `heredoc> ` while waiting for lines. Displaying this prompt can be nice, but check the subject/grading sheet if it's allowed or required.






