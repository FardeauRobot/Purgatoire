# Scripts

Personal CLI tools to boost the workflow.

---

## remind

Broadcast a timed reminder to all your open terminals.

```
remind <time> <unit> <message>
```

| Unit | Meaning  |
|------|----------|
| `s`  | seconds  |
| `m`  | minutes  |
| `h`  | hours    |

**Examples**
```bash
remind 1 h Correction de math
remind 30 m Push avant la pause
remind 10 s Test rapide
```

**What it does**
- Runs in the background (`disown`), so closing the terminal that launched it won't cancel it
- When the time is up, writes the message in **pagga** font with lolcat rainbow colors to every terminal (`/dev/pts/*`) owned by your user
- Also sends a desktop notification via `notify-send` if available

**Dependencies:** `figlet`, `lolcat` (both optional — falls back to plain text)

---

## autopush

Watch the current git repository and automatically commit + push every time you save files.

```
autopush [-m message] [-d minutes] [-b branch]
```

| Option | Default | Meaning |
|--------|---------|---------|
| `-m`   | auto    | Fixed commit message. If omitted, uses `autopush: YYYY-MM-DD HH:MM:SS` |
| `-d`   | `1`     | Debounce delay in **minutes**. Waits this long after the last save before pushing, to avoid a commit per keystroke |
| `-b`   | current | Branch to push to |

**Examples**
```bash
autopush                        # watch current repo, push 1 min after last save
autopush -d 5                   # wait 5 minutes of inactivity before pushing
autopush -m "wip" -b main       # fixed message, push to main
```

**What it does**
- Uses `inotifywait` to watch the entire repo tree for file saves, moves, and deletions (ignores `.git/`)
- Debounces events: if you save 10 files in a row, it waits for the activity to settle before making one single commit
- If there are no actual changes, it skips silently
- Must be run from inside a git repository

**Dependencies:** `inotify-tools`
```bash
sudo apt install inotify-tools
```

---

## note

A quick terminal notepad. Notes are stored in `~/.local/share/notes.md`.

```
note [options] [message]
```

| Command        | Action                  |
|----------------|-------------------------|
| `note "text"`  | Add a note              |
| `note -l`      | List all notes with ids |
| `note -d <id>` | Delete note by id       |
| `note -c`      | Clear all notes         |
| `note`         | Same as `-l`            |

**Examples**
```bash
note "check malloc in ft_split"
note "rebase avant de merger"
note -l
note -d 2
note -c
```

**What it does**
- Timestamps every note automatically (`YYYY-MM-DD HH:MM`)
- Notes persist across sessions in `~/.local/share/notes.md`
- Deleting asks for a valid id (run `note -l` first to see them)
- Clearing asks for confirmation

---

## Other scripts

| Script                  | Purpose |
|-------------------------|---------|
| `cc.sh`                 | Compile C files with 42 flags |
| `AddLibft.sh`           | Build and copy libft into includes/ |
| `clone_libft.sh`        | Clone libft and detach from git |
| `forge_libft.sh`        | Build libft from scratch |
| `solopush.sh`           | Manual add + commit + push in one command |
| `watch_norminette.sh`   | Run norminette on repeat with rainbow output |
| `fopen.sh`              | Open a subject PDF quickly |
| `open_subject.sh`       | Open a subject file |
| `start.sh`              | Open terminal tabs for a project session |
| `divine_compile.sh`     | Compile with special flags |
| `execc.sh`              | Compile and run a C file with ft_putchar |
| `compile_with_putchar.sh` | Compile with putchar support |
| `transcribe_to_github.sh` | Copy project to GitHub repo |
| `copy_to_personal_github.sh` | Copy to personal GitHub |
| `git_add_commit_push.sh` | Git add + commit + push |
| `git_commit_ascend.sh`  | Commit with styled message |
