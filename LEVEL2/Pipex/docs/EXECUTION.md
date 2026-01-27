# Execution Analysis

## Why do you have this output?

The output you see is a result of **Parallel Execution** and **Missing Redirections**.

### 1. The Output Breakdown
```text
PARENT          <-- Parent Loop 1 (After forking Child 1)
PARENT          <-- Parent Loop 2 (After forking Child 2)
/usr/bin/echo   <-- Child 2 Debug Print (in parsing.c)
/usr/bin/ls     <-- Child 1 Debug Print (in parsing.c)
coucou          <-- Child 2 Execution (echo)
total 136       <-- Child 1 Execution (ls)
...
```

### 2. The Explanation
1.  **"PARENT" prints**: Your main process runs a `while` loop. It forks a child, prints "PARENT", loops, forks the next child, prints "PARENT". This happens very fast, often before children have time to start their tasks.
2.  **Debug Paths**: Inside `ft_correct_path`, you have `ft_printf("%s\n", path);`. Since both children run this function to find their command paths, both print to the terminal.
3.  **Command Outputs (`ls` & `echo`)**:
    *   Currently, you are **not using `dup2`** to redirect `STDOUT` (Standard Output) or `STDIN` (Standard Input).
    *   By default, children inherit file descriptors from the parent.
    *   So, both `ls` and `echo` get the **Terminal** as their output destination.
    *   Instead of `ls` writing to the pipe, it writes to the screen.
    *   Instead of `echo` reading from the pipe (if it could), it just prints its arguments to the screen.

---

## Order of Execution

The Operating System scheduler decides exactly when each process runs, but here is the logical flow:

1.  **PARENT**: Starts Loop `i=0`.
2.  **PARENT**: Creates Pipe 0.
3.  **PARENT**: calls `fork()`. **CHILD 1** is created.
4.  **PARENT**: Continues immediately. Prints "PARENT".
5.  **PARENT**: Starts Loop `i=1`.
6.  **PARENT**: Creates Pipe 1.
7.  **PARENT**: calls `fork()`. **CHILD 2** is created.
8.  **PARENT**: Continues immediately. Prints "PARENT".
9.  **PARENT**: Finish loops and exits (or waits, if you added `waitpid`).

*At the same time (concurrently):*

10. **CHILD 1**:
    *   Enters `if (pid == 0)`.
    *   Calls `ft_correct_path("ls")`.
    *   Prints `/usr/bin/ls`.
    *   Calls `execve`. Process becomes `ls`.
    *   `ls` prints file list to Terminal (FD 1).

11. **CHILD 2**:
    *   Enters `if (pid == 0)`.
    *   Calls `ft_correct_path("echo")`.
    *   Prints `/usr/bin/echo`.
    *   Calls `execve`. Process becomes `echo`.
    *   `echo` prints "coucou" to Terminal (FD 1).

---

## Execution Schema

```mermaid
sequenceDiagram
    participant P as Parent Process
    participant C1 as Child 1 (ls)
    participant C2 as Child 2 (echo)
    participant T as Terminal (STDOUT)

    Note over P: Start Loop i=0
    P->>C1: Fork()
    P->>T: Print "PARENT"
    
    Note over P: Start Loop i=1
    P->>C2: Fork()
    P->>T: Print "PARENT"
    
    par Parallel Execution
        rect rgb(200, 220, 240)
            Note over C1: Child 1 runs
            C1->>T: Print "/usr/bin/ls" (Debug)
            C1->>C1: execve("/usr/bin/ls")
            C1->>T: Output "file list..."
        end
        
        rect rgb(220, 240, 200)
            Note over C2: Child 2 runs
            C2->>T: Print "/usr/bin/echo" (Debug)
            C2->>C2: execve("/usr/bin/echo")
            C2->>T: Output "coucou"
        end
    end
```

### Visualizing File Descriptors (Current State vs Goal)

**current State (Wrong):**
```text
[Child 1]  FD 1 (STDOUT) ----------------> [Terminal]
[Child 2]  FD 1 (STDOUT) ----------------> [Terminal]
```

**Goal State (Pipex):**
```text
[Child 1]  FD 1 (STDOUT) ----> [PIPE] ----> [Child 2] FD 0 (STDIN)
                                                |
                                                v
                                           [Child 2] FD 1 (STDOUT) -> [File/Terminal]
```

---

## `waitpid`: comprehensive breakdown

The `waitpid` system call is used to wait for state changes in a child process of the calling process, and obtain information about the child whose state has changed.

### 1. Prototype
```c
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *wstatus, int options);
```

### 2. Parameters Breakdown

#### `pid` (Which child to wait for?)
*   **`<-1`**: Wait for any child process whose process group ID is equal to the absolute value of `pid`.
*   **`-1`**: Wait for **any** child process (same behavior as `wait()` function).
*   **`0`**: Wait for any child process whose process group ID is equal to that of the calling process.
*   **`> 0`**: Wait for the specific child whose process ID is equal to the value of `pid`.

#### `wstatus` (Where to store the exit info?)
*   A pointer to an integer where `waitpid` can store the **status information** of the child.
*   If this is `NULL`, the status information is discarded.
*   You use macros to inspect this integer:
    *   `WIFEXITED(status)`: True if child exited normally.
    *   `WEXITSTATUS(status)`: Returns the exit code (e.g., return value of main or exit() argument).
    *   `WIFSIGNALED(status)`: True if child was terminated by a signal.

#### `options` (How to wait?)
*   **`0`**: The default. The parent process **suspends execution** (blocks) until the child changes state (exits).
*   **`WNOHANG`**: Return immediately if no child has exited. Use this for non-blocking waits.
*   **`WUNTRACED`**: Also return if a child has stopped (e.g., paused by a signal).

### 3. Return Values
*   **`> 0`**: The Process ID (PID) of the child whose state has changed.
*   **`0`**: (Only if `WNOHANG` is used) No child state has changed yet.
*   **`-1`**: An error occurred (e.g., no child processes found relative to the `pid` argument).

### 4. Why is it crucial?

1.  **Synchronization**: It ensures the parent only continues or finishes after the child is done. In a shell or pipe command, you usually want to wait for the command to finish before showing the prompt again.
2.  **Resource Cleanup (Reaping Zombies)**:
    *   When a child finishes, it becomes a **"Zombie"** (Process state `Z`).
    *   It stays in the process table to hold its exit status, just in case the parent wants to read it.
    *   Calling `waitpid` reads this status and tells the OS "I saw it, you can delete this entry now."
    *   Without `waitpid`, zombies accumulate and fill up the process table.

### 5. Standard Usage Schema
```c
pid_t pid = fork();

if (pid == 0) {
    // Child Process
    exit(42);
}
else {
    // Parent Process
    int status;
    waitpid(pid, &status, 0); // Block until child exits
    
    if (WIFEXITED(status))
        printf("Child exited with code %d\n", WEXITSTATUS(status));
}
```
