# PIPEX - Self-Evaluation Test

Here is a set of 10 questions to verify your understanding of the core concepts of the Pipex project, Unix processes, and file descriptors.

---

## Questions

**Q1. `dup2(old, new)` behavior:**
If I call `dup2(fd_A, fd_B)` and `fd_B` is currently open and pointing to a file, what happens to that file connection?

> **Your Answer:** close fd_B and reaffect fd_a to fd_B
>
> **Correction:** ✅ **Correct!**
> Specifically, the kernel **silently closes** `fd_B` first. This is crucial because if you don't realize `fd_B` was open, you might lose data or a connection without warning. After closing, it atomically copies `fd_A` to `fd_B`.

**Q2. `execve` memory:**
If I declare a variable `int counter = 42;` in my main function and then call `execve`, is this variable accessible by the new program? Why?

> **Your Answer:** No, because it doesn't have access to the same part of the memory
>
> **Correction:** ✅ **Correct logic, slightly imprecise terms.**
> It's not just that it "doesn't have access"—it's that the memory **no longer exists**. `execve` completely **wipes** the old memory layout (stack, heap, data) and replaces it with a fresh one for the new program. The variable `counter` is literally deleted from RAM.

**Q3. Pipe Direction:**
In the array `int fd[2]` returned by `pipe()`, which index is for reading and which is for writing?

> **Your Answer:** READING = 0 | WRITE = 1
>
> **Correction:** ✅ **Correct!**
> Mnemotechnic: Standard Input is 0 (Read), Standard Output is 1 (Write).

**Q4. Zombie Processes:**
What specific function call must the parent process make to prevent its children from becoming zombies?

> **Your Answer:** waitpid(pid)
>
> **Correction:** ✅ **Correct!**
> `wait(NULL)` also works for any child. `waitpid` is better because you can wait for a specific child or use non-blocking flags. The kernel keeps the zombie entry until you call this to "read the death certificate".

**Q5. Blocking I/O:**
If a process tries to write 100KB into a pipe but the reader is sleeping, what does the kernel do to the writer?

> **Your Answer:** Puts the reader to sleep and wakes the reader
>
> **Correction:** ❌ **Incorrect.**
> **Correction:** The kernel puts the **WRITER** to sleep (blocks it).
> Since the pipe buffer is small (64KB), writing 100KB fills it up. The writer cannot finish writing. The OS pauses the Writer until the Reader wakes up and consumes some data to make space.

**Q6. `fork()` return values:**
`fork()` returns a `pid_t`. What are the three possible values it can return/cases to handle?

> **Your Answer:** pid = 0 = Child, pid = n = Parent, pid = -1 = Error
>
> **Correction:** ✅ **Correct!**
> Just to be precise: `n` is always positive (`> 0`) and represents the Child's PID.

**Q7. The "Hang" (Deadlock):**
Why does a program hang indefinitely if you forget to close the *write end* of a pipe in the parent process?

> **Your Answer:** Because it expects to find an EOF and stays in an infinite loop 
>
> **Correction:** ✅ **Almost Correct!**
> To be precise: The **Reader** (e.g., `grep` or `wc`) hangs. It keeps calling `read()` because it sees that a write-end is still open (the Parent's), so it thinks data might still come. It will only finish (receive EOF/return 0) when **ALL** write-ends are closed.

**Q8. `access()` vs `open()`:**
Why do we use `access()` with `X_OK` on a command path before calling `execve`, even though `execve` checks permissions itself?

> **Your Answer:** To check if the cmd with path exists before trying to execute it and create undefined behavior or segfault
>
> **Correction:** ⚠️ **Partially True.**
> It does not create undefined behavior or segfault (`execve` handles errors gracefully by returning -1).
> **Real Reason:** We use it to **find** the command. Since we have a list of paths in `$PATH`, we loop through them and use `access()` to quickly check "Is it here?". Once we find the right one, *then* we call `execve` once. Calling `execve` 10 times in a loop until one works is much slower.

**Q9. Environmental Inheritance:**
When `execve` runs a new program, does it automatically inherit the environment variables (like `PATH`) of the old program?

> **Your Answer:** No, we have to test the path for the variables in PATH
>
> **Correction:** ❌ **Incorrect understanding.**
> **Correction:** Using `execve`, **nothing** is inherited automatically. You (the programmer) **MUST** pass the `envp` array explicitly as the 3rd argument: `execve(path, argv, envp)`. If you pass `NULL`, the new program has 0 variables.
> *Note:* You don't have to "test" them, you just have to *pass* the pointer array you received in `main()`.

**Q10. Multiple Commands (Bonus):**
In a pipeline `cmd1 | cmd2 | cmd3`, how many pipes (buffers) are created in total?

> **Your Answer:** 2
>
> **Correction:** ✅ **Correct!**
> `cmd1 --[Pipe A]--> cmd2 --[Pipe B]--> cmd3`.
> Always `N-1` pipes for `N` commands.

---
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>

## Answers

**A1.**
The file connection at `fd_B` is **closed silently** by the system before `fd_B` is made to point to the same resource as `fd_A`.

**A2.**
**No.** `execve` completely replaces the memory space (stack, heap, code) of the current process with the new program. The `counter` variable is erased from memory.

**A3.**
*   `fd[0]` is for **Reading** (Output side).
*   `fd[1]` is for **Writing** (Input side).
*   *Mnemonic: 0 is like stdin (read), 1 is like stdout (write).*

**A4.**
`wait()` or `waitpid()`. These functions read the exit status of the child, allowing the kernel to remove the zombie entry from the process table.

**A5.**
The kernel **blocks** (sleeps/pauses) the writer process until the reader consumes enough data to make space in the pipe buffer (which is typically 64KB).

**A6.**
1.  `0`: We are inside the **Child** process.
2.  `> 0`: We are inside the **Parent** process (value is the Child's PID).
3.  `-1`: The fork **Failed** (error).

**A7.**
The reader process (e.g., `grep`) keeps waiting for data because it only stops when it sees **EOF** (End Of File). EOF on a pipe happens only when **ALL** write ends are closed. If the parent keeps one open, the reader thinks "someone might write more data later" and waits forever.

**A8.**
Technically, you don't *have* to, but:
1.  It allows you to provide a custom, clear error message ("Command not found").
2.  It saves the cost of a failed `execve` syscall (context switch).
3.  It's necessary to iterate through the `$PATH` variable to find *which* folder contains the command.

**A9.**
**No, not automatically.** You must explicitly pass the environment array (`char **envp`) as the third argument to `execve`. If you pass `NULL`, the new program starts with an empty environment.

**A10.**
**2 Pipes.**
*   Pipe 1: Connects `cmd1` to `cmd2`.
*   Pipe 2: Connects `cmd2` to `cmd3`.
