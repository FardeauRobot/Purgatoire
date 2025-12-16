# Pipex Bonus Features

This document covers the bonus features for pipex: multiple pipes and here_doc support.

## Table of Contents

1. [Multiple Pipes](#multiple-pipes)
2. [Here_doc Support](#here_doc-support)

---

## Multiple Pipes

Handle more than 2 commands with multiple pipes.

### Syntax

```bash
./pipex infile "cmd1" "cmd2" "cmd3" "cmd4" outfile
```

**Equivalent to:**
```bash
< infile cmd1 | cmd2 | cmd3 | cmd4 > outfile
```

### Data Flow Visualization

#### 2 Commands (Mandatory)
```
infile → cmd1 → pipe → cmd2 → outfile
```

#### 3 Commands
```
infile → cmd1 → pipe1 → cmd2 → pipe2 → cmd3 → outfile
```

#### 4 Commands
```
infile → cmd1 → pipe1 → cmd2 → pipe2 → cmd3 → pipe3 → cmd4 → outfile
```

#### N Commands Formula
```
Number of pipes needed = Number of commands - 1
```

### Pattern Recognition

For command at index `i` (0-indexed):

| Command Type | Reads From | Writes To |
|--------------|------------|-----------|
| First (i=0) | infile | pipe[0][1] |
| Middle | pipe[i-1][0] | pipe[i][1] |
| Last (i=n-1) | pipe[i-1][0] | outfile |

### Implementation Strategy

```c
int main(int argc, char *argv[], char *envp[])
{
	int		num_cmds;
	int		num_pipes;
	int		pipes[100][2];  // Or allocate dynamically
	pid_t	pids[100];
	int		i;
	
	// Calculate: ./pipex infile cmd1 cmd2 cmd3 outfile
	// argc = 6, commands = [cmd1, cmd2, cmd3] = 3 commands
	num_cmds = argc - 3;
	num_pipes = num_cmds - 1;
	
	// Create all pipes
	for (i = 0; i < num_pipes; i++)
	{
		if (pipe(pipes[i]) < 0)
		{
			perror("pipe");
			exit(1);
		}
	}
	
	// Fork and execute each command
	for (i = 0; i < num_cmds; i++)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("fork");
			exit(1);
		}
		
		if (pids[i] == 0)  // Child process
		{
			// Setup input
			if (i == 0)
			{
				// First command: read from infile
				int infd = open(argv[1], O_RDONLY);
				if (infd < 0)
				{
					perror("open infile");
					exit(1);
				}
				dup2(infd, STDIN_FILENO);
				close(infd);
			}
			else
			{
				// Middle/last commands: read from previous pipe
				dup2(pipes[i - 1][0], STDIN_FILENO);
			}
			
			// Setup output
			if (i == num_cmds - 1)
			{
				// Last command: write to outfile
				int outfd = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (outfd < 0)
				{
					perror("open outfile");
					exit(1);
				}
				dup2(outfd, STDOUT_FILENO);
				close(outfd);
			}
			else
			{
				// First/middle commands: write to next pipe
				dup2(pipes[i][1], STDOUT_FILENO);
			}
			
			// Close ALL pipe file descriptors in child
			for (int j = 0; j < num_pipes; j++)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			
			// Execute command (argv[2+i] is the command)
			execute_command(argv[2 + i], envp);
			exit(127);
		}
	}
	
	// Parent: close all pipes
	for (i = 0; i < num_pipes; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
	}
	
	// Wait for all children
	for (i = 0; i < num_cmds; i++)
	{
		int status;
		waitpid(pids[i], &status, 0);
	}
	
	return (0);
}
```

### Key Points

1. **Create all pipes before forking** - You need N-1 pipes for N commands
2. **Each child closes ALL pipes** - Even the ones it doesn't use
3. **Pattern is consistent** - First reads file, last writes file, middle use pipes
4. **Index arithmetic matters** - Command `i` reads from `pipe[i-1]` and writes to `pipe[i]`

### Testing Multiple Pipes

```bash
# Create test file
echo -e "apple\nbanana\napple\napricot\ngrape" > fruits.txt

# Test with 3 commands
./pipex fruits.txt "cat" "sort" "uniq" output.txt
cat output.txt
# Expected: apple, apricot, banana, grape (sorted, unique)

# Test with 4 commands
./pipex fruits.txt "cat" "grep a" "sort" "uniq" output.txt
cat output.txt
# Expected: apple, apricot, banana, grape (filtered, sorted, unique)

# Compare with shell
< fruits.txt cat | grep a | sort | uniq > expected.txt
diff output.txt expected.txt  # Should match
```

---

## Here_doc Support

Support bash-style here-document input.

### What is Here_doc?

Here_doc allows typing input directly into the terminal until a delimiter word is entered.

**Bash example:**
```bash
cat << EOF
line 1
line 2
line 3
EOF
```

### Pipex Syntax

```bash
./pipex here_doc LIMITER cmd1 cmd2 outfile
```

**Equivalent to:**
```bash
cmd1 | cmd2 >> outfile
# With input from stdin until LIMITER is typed
```

### Example Session

```bash
$ ./pipex here_doc END "grep hello" "wc -l" output.txt
> hello world
> goodbye world
> hello again
> END
$ cat output.txt
2
```

### Key Differences from Normal Mode

| Feature | Normal | Here_doc |
|---------|--------|----------|
| Arguments | 5 total | 6 total |
| First arg | filename | "here_doc" string |
| Second arg | cmd1 | LIMITER word |
| Input source | File | stdin (terminal) |
| Output mode | Truncate | Append |

### Argument Parsing

```c
int main(int argc, char *argv[], char *envp[])
{
	// Check for here_doc mode
	if (argc > 1 && strcmp(argv[1], "here_doc") == 0)
	{
		if (argc != 6)
		{
			ft_putstr_fd("Usage: ./pipex here_doc LIMITER cmd1 cmd2 outfile\n", 2);
			return (1);
		}
		handle_heredoc(argv[2], argv[3], argv[4], argv[5], envp);
	}
	else
	{
		if (argc != 5)
		{
			ft_putstr_fd("Usage: ./pipex infile cmd1 cmd2 outfile\n", 2);
			return (1);
		}
		handle_normal(argv[1], argv[2], argv[3], argv[4], envp);
	}
	
	return (0);
}
```

### Implementation Approach 1: Using Pipe

```c
void	read_heredoc(char *limiter, int write_fd)
{
	char	*line;
	size_t	len;
	
	len = ft_strlen(limiter);
	
	while (1)
	{
		write(STDOUT_FILENO, "heredoc> ", 9);  // Show prompt
		line = get_next_line(STDIN_FILENO);  // Read line
		
		if (!line)  // EOF (Ctrl+D)
			break;
		
		// Check if line matches limiter
		if (ft_strncmp(line, limiter, len) == 0 && 
		    (line[len] == '\n' || line[len] == '\0'))
		{
			free(line);
			break;
		}
		
		// Write line to pipe
		write(write_fd, line, ft_strlen(line));
		free(line);
	}
}

void	handle_heredoc(char *limiter, char *cmd1, char *cmd2,
                       char *outfile, char *envp[])
{
	int		pipefd[2];
	pid_t	pid1, pid2;
	
	// Create pipe for heredoc input
	pipe(pipefd);
	
	// Read input into pipe
	read_heredoc(limiter, pipefd[1]);
	close(pipefd[1]);  // Done writing
	
	// Fork first command
	pid1 = fork();
	if (pid1 == 0)
	{
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		execute_command(cmd1, envp);
		exit(127);
	}
	
	// Create pipe between commands
	int cmd_pipe[2];
	pipe(cmd_pipe);
	
	// Connect child 1 output to cmd_pipe
	// ... (similar to normal pipex)
	
	// Fork second command
	pid2 = fork();
	if (pid2 == 0)
	{
		dup2(cmd_pipe[0], STDIN_FILENO);
		
		// IMPORTANT: Open with O_APPEND
		int outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
		dup2(outfd, STDOUT_FILENO);
		close(outfd);
		close(cmd_pipe[0]);
		close(cmd_pipe[1]);
		
		execute_command(cmd2, envp);
		exit(127);
	}
	
	close(pipefd[0]);
	close(cmd_pipe[0]);
	close(cmd_pipe[1]);
	
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}
```

### Implementation Approach 2: Using Temporary File

```c
void	handle_heredoc_with_file(char *limiter, char *cmd1, char *cmd2,
                                 char *outfile, char *envp[])
{
	int		tmpfd;
	char	*line;
	
	// Create temporary file
	tmpfd = open(".heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	
	// Read input and write to temp file
	while (1)
	{
		write(1, "heredoc> ", 9);
		line = get_next_line(0);
		
		if (!line || ft_strcmp(line, limiter) == 0)
		{
			free(line);
			break;
		}
		
		write(tmpfd, line, ft_strlen(line));
		free(line);
	}
	close(tmpfd);
	
	// Now use temp file as input (but with O_APPEND for output)
	handle_normal_with_append(".heredoc_tmp", cmd1, cmd2, outfile, envp);
	
	// Clean up
	unlink(".heredoc_tmp");
}
```

### Key Points for Here_doc

1. **Check first argument** - Must be exactly the string `"here_doc"`
2. **Show prompt** - Display `"heredoc> "` or `"pipe heredoc> "` for user input
3. **Read until limiter** - Stop when line matches LIMITER exactly
4. **Handle Ctrl+D** - Treat EOF as end of input
5. **Use O_APPEND** - Output file must be appended to, not truncated
6. **Argument count** - 6 total arguments instead of 5

### Testing Here_doc

```bash
# Test basic here_doc
$ ./pipex here_doc EOF "cat" "wc -l" output.txt
heredoc> hello
heredoc> world
heredoc> EOF
$ cat output.txt
2

# Test with grep
$ ./pipex here_doc STOP "grep a" "wc -w" output.txt
heredoc> apple banana
heredoc> cherry apricot
heredoc> STOP
$ cat output.txt
3

# Test append mode (run twice)
$ ./pipex here_doc END "cat" "cat" output.txt
heredoc> first
heredoc> END
$ ./pipex here_doc END "cat" "cat" output.txt
heredoc> second
heredoc> END
$ cat output.txt
first
second

# Test with Ctrl+D
$ ./pipex here_doc NEVER "cat" "wc -l" output.txt
heredoc> line 1
heredoc> line 2
heredoc> ^D
$ cat output.txt
2
```

---

## Combining Both Bonuses

You can implement both features together:

```c
int main(int argc, char *argv[], char *envp[])
{
	// Check for here_doc with multiple commands
	if (argc > 1 && strcmp(argv[1], "here_doc") == 0)
	{
		if (argc < 6)
			return (error_msg("Usage: ./pipex here_doc LIMITER cmd... outfile\n"));
		
		handle_heredoc_multiple(argc, argv, envp);
	}
	// Check for multiple commands
	else if (argc >= 5)
	{
		handle_multiple_pipes(argc, argv, envp);
	}
	else
	{
		return (error_msg("Usage: ./pipex infile cmd1 cmd2 ... outfile\n"));
	}
	
	return (0);
}
```

**Example with both:**
```bash
./pipex here_doc END "grep a" "sort" "uniq" "wc -l" output.txt
heredoc> apple
heredoc> banana
heredoc> apricot
heredoc> apple
heredoc> END
$ cat output.txt
2
```

---

## Summary

### Multiple Pipes
- Support N commands with N-1 pipes
- Pattern: first reads file, middle use pipes, last writes file
- Close ALL pipes in ALL children

### Here_doc
- First arg must be "here_doc" string
- Read from stdin until LIMITER
- Use O_APPEND for output file
- 6 arguments instead of 5

Good luck with the bonus! 🌟