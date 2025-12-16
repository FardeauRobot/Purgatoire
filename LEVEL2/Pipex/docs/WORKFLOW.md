# Pipex Implementation Workflow

Step-by-step guide to implementing pipex from scratch.

## Table of Contents

1. [Project Setup](#project-setup)
2. [Understanding the Goal](#understanding-the-goal)
3. [Implementation Steps](#implementation-steps)
4. [Complete Flow Diagram](#complete-flow-diagram)
5. [Code Examples](#code-examples)
6. [Helper Functions](#helper-functions)
7. [Common Pitfalls](#common-pitfalls)
8. [Data Flow Visualization](#data-flow-visualization)

---

## Project Setup

### File Structure

```
pipex/
├── Makefile
├── pipex.h
├── srcs/
│   ├── main.c
│   ├── parse.c        // Command parsing
│   ├── execute.c      // Command execution
│   ├── path.c         // PATH resolution
│   └── error.c        // Error handling
└── docs/
    ├── README.md
    ├── FUNCTIONS.md
    ├── MACROS.md
    ├── CONCEPTS.md
    ├── WORKFLOW.md     // This file
    ├── BONUS.md
    └── TESTING.md
```

### Required Headers

```c
#include <unistd.h>      // fork, pipe, dup2, execve, close
#include <stdlib.h>      // malloc, free, exit
#include <stdio.h>       // perror
#include <string.h>      // String operations
#include <sys/wait.h>    // wait, waitpid, WIFEXITED, WEXITSTATUS
#include <fcntl.h>       // open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <errno.h>       // errno
```

---

## Understanding the Goal

### What We're Building

```bash
./pipex infile "cmd1" "cmd2" outfile
```

**Must behave exactly like:**
```bash
< infile cmd1 | cmd2 > outfile
```

### Example

```bash
./pipex input.txt "grep hello" "wc -l" output.txt
```

**Is equivalent to:**
```bash
< input.txt grep hello | wc -l > output.txt
```

---

## Implementation Steps

### Step 1: Argument Parsing and Validation

```c
int main(int argc, char *argv[], char *envp[])
{
	// Check argument count
	if (argc != 5)
	{
		ft_putstr_fd("Usage: ./pipex infile cmd1 cmd2 outfile\n", 2);
		return (1);
	}
	
	char *infile = argv[1];
	char *cmd1 = argv[2];
	char *cmd2 = argv[3];
	char *outfile = argv[4];
	
	// Continue to next step...
}
```

**What to validate:**
- Exactly 5 arguments (program name + 4 args)
- Store arguments in meaningful variables

---

### Step 2: File Handling

```c
// Open input file for reading
int infd = open(infile, O_RDONLY);
if (infd < 0)
{
	perror("Error opening input file");
	exit(1);
}

// Open output file for writing (create if needed, truncate if exists)
int outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (outfd < 0)
{
	perror("Error opening output file");
	close(infd);
	exit(1);
}
```

**Important:**
- Check return values (< 0 means error)
- Use appropriate flags (`O_RDONLY` for input, `O_WRONLY | O_CREAT | O_TRUNC` for output)
- Set permissions (0644 = rw-r--r--)
- Clean up on errors

---

### Step 3: Create Pipe

```c
int pipefd[2];

if (pipe(pipefd) < 0)
{
	perror("Pipe creation failed");
	close(infd);
	close(outfd);
	exit(1);
}

// pipefd[0] is the read end
// pipefd[1] is the write end
```

**Remember:**
- `pipefd[0]` = read end
- `pipefd[1]` = write end
- Create pipe BEFORE forking

---

### Step 4: Fork First Child (cmd1)

```c
pid_t pid1 = fork();

if (pid1 < 0)
{
	perror("Fork failed");
	// Clean up...
	exit(1);
}

if (pid1 == 0)  // This is child 1
{
	// CHILD 1: Execute cmd1
	// Reads from infile, writes to pipe
	
	// Redirect stdin from infile
	dup2(infd, STDIN_FILENO);
	
	// Redirect stdout to pipe write end
	dup2(pipefd[1], STDOUT_FILENO);
	
	// Close all file descriptors
	close(infd);
	close(outfd);
	close(pipefd[0]);
	close(pipefd[1]);
	
	// Parse and execute command
	execute_command(cmd1, envp);
	
	// If execve fails, we get here
	perror("Command execution failed");
	exit(127);  // Command not found
}

// Parent continues here...
```

**Key points for child 1:**
1. Redirect stdin from infile
2. Redirect stdout to pipe write end
3. Close ALL file descriptors (including unused pipe read end)
4. Execute command
5. Exit if execve fails

---

### Step 5: Fork Second Child (cmd2)

```c
pid_t pid2 = fork();

if (pid2 < 0)
{
	perror("Fork failed");
	// Clean up...
	exit(1);
}

if (pid2 == 0)  // This is child 2
{
	// CHILD 2: Execute cmd2
	// Reads from pipe, writes to outfile
	
	// Redirect stdin from pipe read end
	dup2(pipefd[0], STDIN_FILENO);
	
	// Redirect stdout to outfile
	dup2(outfd, STDOUT_FILENO);
	
	// Close all file descriptors
	close(infd);
	close(outfd);
	close(pipefd[0]);
	close(pipefd[1]);
	
	// Parse and execute command
	execute_command(cmd2, envp);
	
	// If execve fails, we get here
	perror("Command execution failed");
	exit(127);
}

// Parent continues here...
```

**Key points for child 2:**
1. Redirect stdin from pipe read end
2. Redirect stdout to outfile
3. Close ALL file descriptors (including unused pipe write end)
4. Execute command
5. Exit if execve fails

---

### Step 6: Parent Cleanup and Waiting

```c
// PARENT PROCESS
// Close all file descriptors (no longer needed)
close(infd);
close(outfd);
close(pipefd[0]);
close(pipefd[1]);

// Wait for both children to finish
int status1, status2;
waitpid(pid1, &status1, 0);
waitpid(pid2, &status2, 0);

// Return exit code of last command (cmd2)
if (WIFEXITED(status2))
	return (WEXITSTATUS(status2));

return (1);  // Generic error
```

**Why close in parent?**
- Parent doesn't use the files/pipes
- If parent doesn't close pipe write end, child 2 will never get EOF
- Prevents file descriptor leaks

**Why wait?**
- Prevents zombie processes
- Gets exit codes for error checking
- Ensures commands complete before program exits

---

## Complete Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    PARENT PROCESS                           │
│                                                             │
│  1. Validate arguments (argc == 5)                         │
│  2. Open infile (O_RDONLY)                                 │
│  3. Open outfile (O_WRONLY | O_CREAT | O_TRUNC, 0644)     │
│  4. Create pipe: pipe(pipefd)                              │
│                                                             │
│     pipefd[0] = read end                                   │
│     pipefd[1] = write end                                  │
│                                                             │
│  5. Fork first child (pid1)                                │
│                                                             │
│     ┌──────────────────────────────────────┐              │
│     │     CHILD PROCESS 1 (cmd1)           │              │
│     │                                       │              │
│     │  • Redirect stdin from infile:       │              │
│     │    dup2(infd, STDIN_FILENO)         │              │
│     │                                       │              │
│     │  • Redirect stdout to pipe:          │              │
│     │    dup2(pipefd[1], STDOUT_FILENO)   │              │
│     │                                       │              │
│     │  • Close all file descriptors:       │              │
│     │    close(infd)                       │              │
│     │    close(outfd)                      │              │
│     │    close(pipefd[0])                  │              │
│     │    close(pipefd[1])                  │              │
│     │                                       │              │
│     │  • Execute cmd1:                     │              │
│     │    execve(cmd1_path, args1, envp)   │              │
│     │                                       │              │
│     └──────────────────────────────────────┘              │
│                                                             │
│  6. Fork second child (pid2)                               │
│                                                             │
│     ┌──────────────────────────────────────┐              │
│     │     CHILD PROCESS 2 (cmd2)           │              │
│     │                                       │              │
│     │  • Redirect stdin from pipe:         │              │
│     │    dup2(pipefd[0], STDIN_FILENO)    │              │
│     │                                       │              │
│     │  • Redirect stdout to outfile:       │              │
│     │    dup2(outfd, STDOUT_FILENO)       │              │
│     │                                       │              │
│     │  • Close all file descriptors:       │              │
│     │    close(infd)                       │              │
│     │    close(outfd)                      │              │
│     │    close(pipefd[0])                  │              │
│     │    close(pipefd[1])                  │              │
│     │                                       │              │
│     │  • Execute cmd2:                     │              │
│     │    execve(cmd2_path, args2, envp)   │              │
│     │                                       │              │
│     └──────────────────────────────────────┘              │
│                                                             │
│  7. Close pipes and files in parent:                       │
│     close(infd)                                            │
│     close(outfd)                                           │
│     close(pipefd[0])                                       │
│     close(pipefd[1])                                       │
│                                                             │
│  8. Wait for children:                                     │
│     waitpid(pid1, &status1, 0)                            │
│     waitpid(pid2, &status2, 0)                            │
│                                                             │
│  9. Return exit code of last command:                      │
│     return (WEXITSTATUS(status2))                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Code Examples

### Minimal Working Example

```c
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>

void	execute_command(char *cmd, char **envp)
{
	// Simplified - you need to parse cmd and find in PATH
	char *args[] = {cmd, NULL};
	execve(cmd, args, envp);  // Needs full path!
}

int	main(int argc, char **argv, char **envp)
{
	int		pipefd[2];
	pid_t	pid1, pid2;
	int		infd, outfd;
	
	if (argc != 5)
		return (write(2, "Usage: ./pipex infile cmd1 cmd2 outfile\n", 41), 1);
	
	// Open files
	infd = open(argv[1], O_RDONLY);
	outfd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	pipe(pipefd);
	
	// Fork child 1
	pid1 = fork();
	if (pid1 == 0)
	{
		dup2(infd, 0);
		dup2(pipefd[1], 1);
		close(infd);
		close(outfd);
		close(pipefd[0]);
		close(pipefd[1]);
		execute_command(argv[2], envp);
		exit(127);
	}
	
	// Fork child 2
	pid2 = fork();
	if (pid2 == 0)
	{
		dup2(pipefd[0], 0);
		dup2(outfd, 1);
		close(infd);
		close(outfd);
		close(pipefd[0]);
		close(pipefd[1]);
		execute_command(argv[3], envp);
		exit(127);
	}
	
	// Parent cleanup
	close(infd);
	close(outfd);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	
	return (0);
}
```

---

## Helper Functions

### 1. Parse Command

```c
char	**parse_command(char *cmd)
{
	// Split "grep -i hello" into ["grep", "-i", "hello", NULL]
	// Use ft_split or similar
	return (ft_split(cmd, ' '));
}
```

### 2. Find Command in PATH

```c
char	*find_in_path(char *cmd, char **envp)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;
	
	// If cmd is already a path (starts with / or ./)
	if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	
	// Get PATH from environment
	path_env = get_path_from_env(envp);
	if (!path_env)
		return (NULL);
	
	// Split PATH by ':'
	paths = ft_split(path_env, ':');
	
	// Try each directory
	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(full_path, cmd);
		
		if (access(full_path, X_OK) == 0)
		{
			free_array(paths);
			return (full_path);
		}
		
		free(full_path);
		i++;
	}
	
	free_array(paths);
	return (NULL);
}
```

### 3. Get PATH from Environment

```c
char	*get_path_from_env(char **envp)
{
	int	i;
	
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);  // Skip "PATH="
		i++;
	}
	return (NULL);
}
```

### 4. Execute Command

```c
void	execute_command(char *cmd, char **envp)
{
	char	**args;
	char	*path;
	
	// Parse command into arguments
	args = parse_command(cmd);
	if (!args || !args[0])
		exit(127);
	
	// Find command in PATH
	path = find_in_path(args[0], envp);
	if (!path)
	{
		ft_putstr_fd("command not found: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putchar_fd('\n', 2);
		free_array(args);
		exit(127);
	}
	
	// Execute
	execve(path, args, envp);
	
	// If we get here, execve failed
	perror("execve");
	free(path);
	free_array(args);
	exit(126);
}
```

---

## Common Pitfalls

### 1. Forgetting to Close Pipe Ends

❌ **Wrong:**
```c
// Child 1
dup2(pipefd[1], STDOUT_FILENO);
// Forgot to close pipefd[0] and pipefd[1]!
```

✅ **Correct:**
```c
// Child 1
dup2(pipefd[1], STDOUT_FILENO);
close(pipefd[0]);  // Not using read end
close(pipefd[1]);  // Close after redirecting
```

**Why:** If you don't close unused pipe ends, child 2 will never receive EOF because there's still a write end open (in child 1).

---

### 2. Not Handling PATH Correctly

❌ **Wrong:**
```c
char *args[] = {"cat", NULL};
execve("cat", args, envp);  // Will fail - needs full path!
```

✅ **Correct:**
```c
char *args[] = {"cat", NULL};
char *path = find_in_path("cat", envp);  // Returns "/bin/cat"
execve(path, args, envp);
```

---

### 3. Memory Leaks Before execve

⚠️ **Debatable:**
```c
char *path = malloc(100);
// ...use path...
execve(path, args, envp);  // Didn't free path
```

**Note:** If execve succeeds, the process is replaced so memory is freed anyway. But if it fails, you have a leak.

✅ **Better:**
```c
char *path = find_in_path(cmd, envp);
execve(path, args, envp);
// If we get here, execve failed
free(path);
exit(127);
```

---

### 4. Wrong Exit Codes

❌ **Wrong:**
```c
if (WIFEXITED(status2))
	return (0);  // Always returns success!
```

✅ **Correct:**
```c
if (WIFEXITED(status2))
	return (WEXITSTATUS(status2));  // Return actual exit code
```

---

## Data Flow Visualization

```
Input File (infile)
       │
       │ open() → infd
       ▼
   ┌─────────┐
   │  CHILD  │
   │    1    │  dup2(infd, STDIN)
   │  (cmd1) │  dup2(pipe[1], STDOUT)
   └─────────┘
       │
       │ Data flows through pipe
       ▼
   ┌─────────┐
   │  PIPE   │  [pipe[0] ← pipe[1]]
   └─────────┘
       │
       │ Data flows to child 2
       ▼
   ┌─────────┐
   │  CHILD  │
   │    2    │  dup2(pipe[0], STDIN)
   │  (cmd2) │  dup2(outfd, STDOUT)
   └─────────┘
       │
       │ open() → outfd
       ▼
Output File (outfile)
```

---

## Next Steps

1. **Implement the basic mandatory part** using this workflow
2. **Test thoroughly** (see TESTING.md)
3. **Add error handling** for edge cases
4. **Implement bonus features** (see BONUS.md)

Good luck! 🚀
