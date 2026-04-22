# Scripts

Personal CLI tools to boost the workflow. All live in `Scripts/` at the repo root — add it to your `PATH` (`export PATH="$HOME/Desktop/BOULOT/Purgatoire/Scripts:$PATH"`) and they're callable from anywhere.

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

## solopush

Manual one-shot version of autopush. Stages everything, commits with your message, pushes to the current branch.

```
solopush "commit message"
```

**Examples**
```bash
solopush "Fix bug in login function"
solopush "Add new feature"
```

**What it does**
- Refuses to run outside a git repo
- Exits cleanly if there are no changes
- Shows the status before/after the commit
- Falls back to `git push -u origin <branch>` if the first push fails (handles first push of a new branch)

Use `solopush` when you want to think about the message. Use `autopush` when you don't.

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

## cc42

Compile C files with the 42 flag trio (`-Wall -Wextra -Werror`) and a configurable `BUFFER_SIZE` macro — useful for GNL-style exercises.

```
cc42 [-b BUFFER] [-o output] <files...>
```

| Option | Default | Meaning |
|--------|---------|---------|
| `-b`   | `42` (or `$CC_BUFFER`) | Value passed via `-DBUFFER_SIZE=...` |
| `-o`   | `a.out` | Executable name |

**Examples**
```bash
cc42 -b 100 main.c
cc42 -o test prog.c utils.c
CC_BUFFER=32 cc42 main.c
```

After building, prompts to run the resulting executable.

---

## runc

Compile a C file alongside `ft_putchar.c` from `~/Desktop/Labo/` — handy for Piscine exercises that require `ft_putchar` but don't ship it.

```
runc <filename.c> [output_name]
```

**Examples**
```bash
runc main.c
runc ex00/ft_print_alphabet.c my_prog
```

Output name defaults to the input's basename without `.c`. Prompts to run after a successful build.

---

## libft-add

Build `libft.a` from the repo's `libft/` and copy the whole folder into `./includes/libft/` of your current project.

```
libft-add [make-target]
```

Defaults to `make all`; pass `re` or `clean` to override.

**What it does**
- Runs `make -C <libft>` with the chosen target
- Copies the built folder (with `libft.a` and headers) into `includes/libft/`, so your project can link against it without relying on the submodule path

---

## libft-clone

Clone [`libft_official`](https://github.com/FardeauRobot/libft_official) into `./includes/libft/` and strip its `.git` so it's a flat, unlinked copy.

```
libft-clone
```

If `includes/libft/` already exists, prompts before replacing it. Use this when you want a standalone copy (e.g. before submission) rather than a submodule linkage.

---

## cppinit

Bootstrap a new C++ project from one of the templates under `templates/cpp/`.

```
cppinit <basic|complete> <project_name>
```

| Variant    | Layout                                           |
|------------|--------------------------------------------------|
| `basic`    | `src/` + `include/`                              |
| `complete` | `src/core/`, `src/utils/`, more scaffolding      |

Creates the project in the current directory.

---

## watch-norm

Run `norminette` repeatedly on a path, with rainbow output if `lolcat` is available. Think `watch` for the Norm.

```
watch-norm [-n interval] [-L] [-r] [path]
```

| Option | Default | Meaning |
|--------|---------|---------|
| `-n`   | `15`    | Seconds between runs |
| `-L`   | off     | Force `lolcat` piping |
| `-r`   | off     | Strip norminette colors first, then re-rainbow |
| path   | `.`     | File or directory to check |

**Examples**
```bash
watch-norm
watch-norm -n 5 src
watch-norm -L .
```

---

## open-pdf

Quickly open a subject PDF by name (no `.pdf` extension needed).

```
open-pdf <subject-name>
```

Searches `$HOME/Documents/Subjects` (edit the `SCRIPTS_PATH` variable in the script if your PDFs live elsewhere — the repo's `Subjects/` folder is a good candidate). Lists available PDFs when called with no argument. Tries `evince`, `okular`, `xdg-open`, then `firefox`.

---

## start

Open a gnome-terminal window with three tabs prewired: Spotizer visualizer, Spotify Web, and 42 Intra. Assumes you have the corresponding shell aliases/functions (`Spotizer`, `Spot`, `PlaySpot`, `Intra`).

```
start
```

Pure convenience — gnome-terminal specific.
