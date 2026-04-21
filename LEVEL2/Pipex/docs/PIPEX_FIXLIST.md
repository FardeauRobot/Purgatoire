# Pipex Fix Checklist

Use this as a punch list to make your pipex robust across edge cases. File/line links assume current layout.

## Critical runtime bugs
- **Wrong pipe end for stdin in children**: In [srcs/main.c](srcs/main.c#L131-L158), `dup2(pipe_arr[i - 1][WRITE], STDIN_FILENO)` should use the **READ** end for non-first commands. Fix to `pipe_arr[i - 1][READ]`.
- **Pipes not closed in children**: After `dup2` in [ft_exec_child](srcs/main.c#L131-L158), close **all** pipe fds (`2 * (cmd_nbr - 1)`) inherited by the child to avoid deadlocks.
- **Double close + no guard on fds**: [ft_exec_child](srcs/main.c#L131-L158) calls `ft_close_files` then closes `outfile_fd`/`infile_fd` again; remove duplicates and only close valid fds.
- **Path check with NULL**: In [ft_exec_pipes](srcs/main.c#L199-L224), `access(pipex->cmds[i]->path, ...)` is called even when `path` might be `NULL`. Add a NULL check before `access`, and return 127 for "command not found".

## Waiting / exit codes
- **Wait loop increments twice**: In [ft_exec_pipes](srcs/main.c#L214-L224), the loop `while (i++ < cmd_nbr) { ...; i++; }` skips waits. Remove the inner `i++`.
- **Return shell-like exit codes**: If a command is not found, exit 127; if permission denied, exit 126; propagate last child status; handle signals via `128 + signal` (already partly present).

## File handling
- **Heredoc vs truncate**: For here_doc mode, open outfile with `O_APPEND`; for normal mode, keep `O_TRUNC`. Adjust in [ft_default_pipex](srcs/main.c#L66-L125).
- **Missing infile error path**: When `open(infile)` fails, still close unused pipes, set exit to 1/errno, and avoid using fd `-1` in `dup2`.
- **PATH discovery robustness**: [ft_find_path_env](srcs/main.c#L33-L60) leaves `path` uninitialized if PATH is missing. Detect absence and error out cleanly.

## Command parsing
- **Quoted args / special chars**: `ft_split_charset_gc` likely breaks quoted segments (`"arg with spaces"`, `'*.c'`). Either implement simple quote-aware splitter or document limitation. Add tests.
- **Empty command tokens**: Reject empty command (e.g., `""`), return 127.

## Pipes & fds hygiene
- **Close unused ends in parent early**: After each fork, parent can close the child-specific pipe ends to reduce fd pressure (optional but cleaner).
- **Validate pipe allocation**: Ensure `pipe_arr` allocation handles `cmd_nbr == 1` (no pipes) without invalid malloc sizes.

## Bonus (multi-cmd) specifics
- **Generalize child setup**: For middle commands, stdin = previous pipe READ, stdout = next pipe WRITE. First uses infile/pipe, last uses pipe/outfile.
- **Looped pipe open**: Keep `ft_open_pipes` but guard when `cmd_nbr < 2`.

## Error reporting
- Consistent error messages to stderr; use `perror` for syscalls; avoid double-printing.
- Ensure `error_exit` does not free `envp`/static data; but frees gc allocations and closes fds.

## Memory / leaks
- Free all GC allocations on exit (`ft_gc_free_all` or equivalent). Ensure children don’t double-free shared structures.
- No leaks on failed path resolution or split failures.

## Tests to re-run after fixes
- `./test_pipex.sh` (all 20 tests), especially: missing infile, invalid cmd, bonus multi-pipe, heredoc truncate/append, large input, binary input, valgrind (test 20).
