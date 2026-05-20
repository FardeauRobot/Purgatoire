# CLAUDE_CODE_MANUAL.md

A short, practical manual for using **Claude Code** efficiently — tailored for the 42 piscine context (C++98, macOS, norminette, valgrind).

The order goes **setup → prompts → commands → workflows → 42-specific tips → pitfalls → power moves**. Skim once, come back as needed.

---

## 1. Setup once, save time forever

The biggest efficiency gains come from **reducing friction**, not from clever prompts.

### `CLAUDE.md` at repo root
Project-level instructions Claude reads automatically every session. Run `/init` to bootstrap one. Put in it:
- Build commands (`make`, `make re`, `./prog`)
- Conventions (C++98 only, no STL containers in early modules, norminette rules)
- Folder layout (where exercises live)

**Why:** without it, you re-explain "we're in C++98, no `auto`, no `nullptr`" every single session.

### Auto-memory (already active)
Tell Claude *"remember that I prefer X"* and it persists **across sessions**. Stored in `~/.claude/projects/.../memory/`.
- Audit it: ask *"what do you remember about me?"*
- Update it: *"forget X"* or *"remember Y instead"*

### Permissions allowlist
Run the `fewer-permission-prompts` skill once — it scans your transcripts and adds your common commands (`make`, `c++`, `norminette`, `valgrind`) to `.claude/settings.json`. No more clicking "allow" 50× a day.

---

## 2. Write good prompts

| ❌ Vague | ✅ Specific |
|---|---|
| "fix this" | "in `Mod01/ex01/main.cpp:23`, the destructor isn't called on heap zombies — explain why and fix" |
| "make a class" | "create a `Fixed` class per Mod02/ex00 subject, in C++98, with the four canonical members" |
| "it crashes" | "`./prog` segfaults after the third input. Here's the valgrind output: [paste]" |

**The recipe:** *Where* (file:line) + *What* (fix/explain/refactor) + *Why or what you tried*.

---

## 3. Essential slash commands

| Command | When to use |
|---|---|
| `/clear` | Start fresh — context cluttered or topic switch |
| `/init` | Generate a CLAUDE.md for a new repo |
| `/config` | Switch model, output style, theme |
| `/review` | Review a PR or branch |
| `/security-review` | Audit pending changes |
| `/help` | Built-in help |

Type `/` to see everything, including custom skills (`simplify`, `loop`, `schedule`).

---

## 4. Workflow patterns

### Plan first for big changes
> *"plan how to refactor X across all modules. Don't edit yet."*

Review the plan, then say "go." Avoids 200 lines of wrong code.

### Subagents for research
> *"spawn an Explore agent to find every place we use `std::string` in this repo"*

Keeps your main context clean — the subagent reads files, returns a summary.

### One task at a time
Don't pile 5 features into one prompt. Smaller turns = fewer mistakes and easier rollback.

### `!` prefix for shell commands
Typing `! make re && ./zombie` runs the command in your session and feeds output to Claude in one shot. Far better than copy-pasting compile errors.

### Drag & drop
Files, screenshots, valgrind logs — drop them straight into the prompt. Claude reads them.

---

## 5. For your 42 piscine specifically

- Keep **Explanatory style on** for piscine work (`/config` → output style). You get reasoning + insights, not just code dumps.
- Ask *"explain WHY this is C++98-compatible"* to lock in the constraint.
- Norm violations? Paste norminette output — Claude fixes them faster than describing them.
- Memory bugs? Run with `valgrind --leak-check=full` and paste output.
- Use your library notes as context: *"read `library/cpp/oop/GETTERS_SETTERS.md` and apply that style here"*.
- For canonical form (orthodox canonical four), ask Claude to **list all four members and verify each is implemented** — easy to forget the assignment operator.

---

## 6. Pitfalls to avoid

- ❌ Long monologues with no question — Claude can't read your mind
- ❌ "Continue" / "go on" with no direction — be specific
- ❌ Letting context fill up — `/clear` between unrelated tasks
- ❌ Trusting blindly — always read the diff before approving destructive ops (`git push`, `rm`, force-push)
- ❌ Asking Claude to write code without first asking it to *understand* the existing code

---

## 7. Power moves

- **Esc twice** → jump back to a previous message and edit it. Far better than retyping.
- **Memory check**: *"what do you remember about me?"* → audit stored memories.
- **Background loops**: `/loop 5m /review` runs review every 5 min while you keep coding.
- **Multi-turn debugging**: keep the same conversation when iterating on one bug — Claude builds up understanding session-by-session.
- **Parallel tool calls**: when you ask for multiple independent things, Claude runs them in parallel. Bundle requests (*"read these 3 files and tell me what they share"*) instead of asking one by one.
- **Context discipline**: long conversations get truncated at the front. Save important state in CLAUDE.md or memory before that happens.

---

## 8. Quick reference: when to use what

| Need | Use |
|---|---|
| Persist info across sessions | Auto-memory (`remember that…`) |
| Project conventions for everyone | `CLAUDE.md` |
| Track multi-step work this session | TodoWrite (Claude does this automatically) |
| Plan a complex change | Plan mode (ask Claude to plan first) |
| Save command output to file | `! command > file` |
| Reset context | `/clear` |
| Switch model (Opus / Sonnet / Haiku) | `/config` |

---

## 9. Mental model

Claude Code is **a pair programmer who can read your whole repo, run your tools, and remember your preferences**. The more it knows about your project (CLAUDE.md), your habits (memory), and the specific task (good prompts), the better it gets.

> The single biggest unlock is realizing that the *first 10 minutes of setup* (CLAUDE.md + permissions + memory) saves *hours per week* afterwards.
