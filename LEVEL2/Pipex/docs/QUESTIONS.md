# PIPEX: Concept Mastery Questionnaire

Use this file to test your understanding of the core concepts before or after coding Pipex. Write your answers in the spaces provided.

---

## I. Processes & Execution (`fork`, `execve`, `wait`)

**Q1. What specific resource does `fork()` duplicate, and what does it *not* duplicate?**  
*(Hint: Think about memory, file descriptors, PIDs)*

**Answer:**
<!-- Write your answer here -->

It duplicates the data except the functions which stay in the same storage. Pid for the children is 0 so it doesn't cpy the PID and gets everything else

> **Correction / Refinement:**
> `fork()` duplicates the **entire memory space** (Heap, Stack, Data segments) and the **File Descriptors table**.
> It does **NOT** duplicate:
> - The **PID** (Child gets a new unique ID).
> - The **PPID** (Parent ID).
> - **Pending signals** or file locks.
> 
> *Clarification:* The functions (Code/Text segment) are usually shared in read-only mode (Copy-On-Write) for efficiency, so you are right in spirit.
> *Note:* The child's PID is not 0. `fork()` *returns* 0 to the child process to let it know it is the child. The child has its own positive integer PID.


**Q2. Why is `execve` often called a "Brain Transfer"? What happens to the memory of the process that calls it?**

**Answer:**
<!-- Write your answer here -->

Execve erases the memory of the existing process and replaces it with new one. This is why it's called brain trasnfer

> **Correction / Refinement:**
> **Correct.** `execve` completely replaces the current process's image (Code, Stack, Heap, Data) with the new program loaded from the executable file. The PID remains the same, but the "brain" is swapped.


**Q3. If `execve` is successful, what line of code executes next in that function?**

**Answer:**
<!-- Write your answer here -->
the first line of the cmd given in execve

> **Correction / Refinement:**
> **Technically:** No line of code *in the calling function* executes next.
> The process jumps to the **entry point** of the *new program* (usually `main`).
> The code specifically *after* the `execve` call in your C file is **never reached** if `execve` is successful. If it *is* reached, `execve` failed.


**Q4. What is a "Zombie Process"? Precisely what system call prevents them?**

**Answer:**
<!-- Write your answer here -->

Zombie process happens when a child is lost in the process because an error was produced during the transfer. Per example if there's an saturation of the buffer between the write / read

> **Correction / Refinement:**
> **Incorrect.** A Zombie is **not** an error state caused by buffer saturation.
> A Zombie is a process that has **finished execution** (exited), but its parent has not yet asked for its status. It is "dead", but its entry still hangs around in the process table holding its exit code.
> **Solution:** The parent must call `wait()` or `waitpid()` to "reap" the zombie and read the exit code.

---

## II. File Descriptors & Pipes (`dup2`, `pipe`)

**Q5. In the context of `dup2(oldfd, newfd)`, if `newfd` was already open (e.g., pointing to a file), what happens to it before the duplication?**

**Answer:**
<!-- Write your answer here -->
It closes it

> **Correction / Refinement:**
> **Correct.** `dup2` silently closes `newfd` if it was open, then makes `newfd` point to the same file description as `oldfd`.


**Q6. A pipe created with `pipe(fd)` has two ends. Which index (`fd[0]` or `fd[1]`) is for reading, and which is for writing?**

**Answer:**
<!-- Write your answer here -->

0 is for read
1 for write

> **Correction / Refinement:**
> **Correct.** 
> Tip: Remember **STDIN (0)** is usually input/read, **STDOUT (1)** is usually output/write. The pipe indices follow this convention.


**Q7. Explain why the Parent process must close the write-end of the pipe after forking the children. What specific condition does the reading Child wait for?**

**Answer:**
<!-- Write your answer here -->
The parent process has to close the write end because the children will never begin to work, because of an infinite loop. Reading child expects to find an EOF signal

> **Correction / Refinement:**
> **Correct Logic.**
> The reading child will block (wait) inside `read()` as long as **ANY** file descriptor pointing to the write-end of the pipe remains open (including the one in the parent).
> Removing the last write-reference sends **EOF** (End Of File, return 0) to the reader.


---

## III. System Architecture

**Q8. Does a pipe exist on the Hard Disk or in RAM?**

**Answer:**
<!-- Write your answer here -->
Pipe exists in the RAM

> **Correction / Refinement:**
> **Correct.** A pipe is a buffer in the kernel's memory (RAM).

**Q9. If a process tries to write to a pipe that is full (typically 64KB), what does the kernel make the process do?**

**Answer:**
<!-- Write your answer here -->
It will shut down the process and return an error, then accessible with different macros such as WIFEXITSTATUS 

> **Correction / Refinement:**
> **Incorrect.** The process does **not** crash or error.
> The kernel **BLOCKS** (pauses/sleeps) the writing process until the reading process reads some data, freeing up space in the buffer.
> *Note:* This is why `pipex` commands must run in parallel (processes) or be handled carefully—if the writer fills the pipe before the reader starts reading, everyone might hang (deadlock).


**Q10. How does the shell (and your program) know that `ls` actually means `/usr/bin/ls`? Explain the steps.**

**Answer:**
<!-- Write your answer here -->

the shell knows what ls means because it's a program located in the directories of the computer. It will look for an existing program with every possible path given to him.
Finally it will execute the program if the authorizations are good

> **Correction / Refinement:**
> **Correct.**
> **Correction / Refinement:**
> This is a symptom. The **Cause** is usually **Not closing an unused pipe write-end**.
> If execute `cmd1 | cmd2`, `cmd2` reads from the pipe. If the parent (or `cmd1`) leaves the write-end open, `cmd2` thinks more data is coming and waits forever for EOF.

**Q12. Your program prints the output of the second command, but it's empty. The first command worked fine. What happened to the data in the pipe?**

**Answer:**
<!-- Write your answer here -->
No idea

> **Correction / Refinement:**
> Several possibilities:
> 1.  The second command **does not read from Stdin** (e.g., executing `ls` as the second command ignores the pipe input).
> 2.  The first command failed silently or wrote to Stderr.
> 3.  **Fd Leak/Logic:** The output of `cmd1` was written to the pipe, but `cmd2`'s input wasn't redirected to the read-end properly using `dup2`.You run your pipex, and it hangs forever (doesn't exit). What is the #1 most likely cause involving file descriptors?**

**Answer:**
<!-- Write your answer here -->
Child waiting indefinitely



**Q12. Your program prints the output of the second command, but it's empty. The first command worked fine. What happened to the data in the pipe?**

**Answer:**
<!-- Write your answer here -->
No idea



