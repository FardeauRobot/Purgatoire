# Thinking & Effort in Claude — session summary

A ground-up explanation of what "thinking" and "effort" mean for an AI model,
how effort changes token cost and output, how the model judges task difficulty,
and which effort level to use for which task on Opus vs Sonnet.

---

## 1. What "thinking" is for an AI

A language model generates text one token (~word-piece) at a time. **Thinking**
(Anthropic's term: *extended thinking*) is when the model first generates a stretch
of **private reasoning text** — a scratchpad — before writing the answer you see.

- It's the same idea as a student showing their work instead of blurting the answer.
- The model writes things like *"the question asks X, but there's a catch with Y,
  let me check the edge case..."*, then writes the clean final answer.
- You normally don't see the scratchpad (hidden, or shown only as a summary), but
  the model produced it, and producing it is what improves the final answer on hard
  problems.

**Why it helps:** a model has no workspace other than the text it has already
produced. Writing intermediate steps *is* how it works through a problem. More steps
= more chances to catch a mistake, try an alternative, decompose something complex.
On easy questions it adds nothing; on multi-step reasoning it matters a lot.

---

## 2. What "effort" is

**Effort is a dial for how much thinking the model does before answering.**

- On **Opus** and **Sonnet** the levels are: `low`, `medium`, `high`, `xhigh`, `max`.
- Default is `high` on both (equivalent to not setting it).
- `low` → think briefly or not at all, answer fast.
- `max` → think extensively: explore alternatives, double-check, work the hard cases
  before committing.

Effort does **not** change the model. Opus at `low` and Opus at `max` are the
identical model with identical knowledge — only the amount of pre-answer reasoning
differs. Effort also slightly shapes the visible answer: lower effort → terser
replies, less preamble, more consolidated tool use.

---

## 3. How effort changes token consumption

Every API request is billed on tokens:

| Component | What it is | Affected by effort? |
|---|---|---|
| **Input tokens** | your prompt + conversation history | no |
| **Output tokens** | everything the model generates — **including the thinking scratchpad** | **yes, a lot** |

- The thinking scratchpad is billed at the **output-token rate** (Opus 5: $25 / 1M;
  Sonnet 5: $10 / 1M).
- Going `low` → `max` on a genuinely hard prompt can multiply the output-token bill
  several-fold (thousands of extra scratchpad tokens).
- On an easy prompt the difference is small — the model won't burn a big budget it
  doesn't need. Effort is a ceiling / tendency, not a quota.
- **Latency scales the same way:** more thinking = longer wait before the answer.

### Comparing Opus at `low` vs `max` (method)

Run the *same prompt* through the same model twice, changing only effort. The API
returns a `usage` object with token counts on every response. Measure per run:

1. **Cost** — total output tokens (thinking + visible answer). Close on easy prompts,
   far apart on hard ones.
2. **Latency** — wall-clock to the answer.
3. **Quality** — did it get the right answer? Needs prompts with known answers or a
   rubric. Use a **mix of easy and genuinely hard** problems — the hard tail is the
   only place the difference shows.

Typical pattern (from Anthropic's published measurements):

- **Easy / routine work** (chat, classification, straightforward Q&A): quality curve
  is nearly flat. `low` gives up ~1–3 points of accuracy for ⅓–½ off cost, and is
  faster. `max` not worth it.
- **Hard multi-step work** (tricky coding, long agentic tasks, deep reasoning): a real
  trade-off. Long-horizon coding on Opus 5: ~2 points lost at `medium` for half the
  cost; ~8 points lost at `low` for a quarter of the cost.
- **Diminishing returns at the top:** `max` only pulls ahead of `high` / `xhigh` on
  the very hardest problems. Raise to `max` only after measuring that the level below
  leaves quality on the table.

Cheap proxy without formal scoring: run the same hard prompt ~5× at each level and
eyeball answer consistency and error rate.

---

## 4. How the model tells a hard task from an easy one, and when it stops

### There is no separate difficulty detector

Nothing classifies your prompt as "easy" or "hard" up front. The judgment is
**emergent from the model itself** and happens *while* it works.

During training the model was rewarded for getting correct answers **efficiently** —
reasoning a lot when it changed the outcome, not reasoning when it didn't. It learned
to read the same difficulty cues a person would:

- Did a plausible answer come to mind immediately, or not?
- Multiple parts? Constraints that fight each other?
- Does a first attempt in the scratchpad surface a complication?
- Is the question ambiguous?

So the assessment is **continuous and self-correcting**: the model can start
thinking, discover the problem is deeper than it looked, and keep going — or find
it's routine and wrap up early. This is "adaptive thinking": the model dynamically
decides *when* and *how much*.

### What effort changes

Effort doesn't hand the model a rule or a token count. It **shifts where the model
draws the "I'm done thinking now" line**:

- `low` → stop as soon as there's a plausible answer, even with some uncertainty left.
- `max` → don't stop there: check it, try another approach, consider edge cases, hunt
  for the mistake, even when the current answer looks right.

Within any setting the model still spends more on a hard prompt than an easy one.
Effort moves the whole curve up or down; it doesn't flatten it.

**Analogy:** a chess player under a time control. Blitz vs classical is the same
player with the same skill — the time budget changes how many candidate moves they
check before committing. In either control they still spend longer on a sharp
tactical position than on an obvious recapture.

### How "stop" actually happens

The model generates scratchpad tokens and at some point emits an internal signal
ending the thinking phase and switching to the visible answer. That "keep thinking
vs answer now" decision is itself learned behavior, tuned by the effort level, which
was baked in during training by rewarding the model across different effort budgets.

It's a **soft** mechanism — effort biases a judgment, it's not a hard ceiling. Hard
ceilings are separate features:
- `max_tokens` — an enforced cut-off the model can't see.
- *task budgets* — an explicit token allowance the model paces itself against.

### It isn't perfect

The model can misjudge — stop too early on something subtle, or grind on something
simple. Hence a common cost pattern: **run everything at `low`, auto-re-run only the
failures at higher effort.** It's an admission that the model's own difficulty
estimate is good but not reliable.

---

## 5. Effort levels — examples and use cases (Opus & Sonnet)

Effort means the same on both models. Difference: **Opus has a higher ceiling on hard
reasoning**, so on a given task Sonnet often needs one notch *more* effort to match
Opus — and on the very hardest tasks Sonnet won't get there at any effort (signal to
switch models, not to crank effort).

### At a glance

| Effort | For | Cost vs `high` | Typical use |
|---|---|---|---|
| **low** | Snap answers, mechanical work | ~¼–⅓ | classification, extraction, simple edits, subagents |
| **medium** | Routine work with some judgment | ~⅔–¾ | summarizing, drafting, small code reviews, support answers |
| **high** *(default)* | General problem-solving | baseline | feature coding, debugging, design questions, analysis |
| **xhigh** | Long agentic sessions, hard coding | ~1.5–2× | multi-file changes with tests, big-codebase debugging |
| **max** | Correctness over cost | ~2–4× | security/concurrency bugs, proofs, prod migrations, deep research |

### `low`
Barely thinks — almost straight to the answer.
- "Which of these 5 categories does this ticket belong to?"
- Pull line items and totals out of invoice text
- "Convert this JSON to YAML" / fix markdown formatting
- "What does the `--squash` flag do?"
- Subagent grunt work: "read these 30 files, list the ones that import `requests`"
- One-line code edits with a clear spec: "add a null check on line 40"
- Quick tone / wording tweaks

**Opus vs Sonnet:** Sonnet's sweet spot. Opus `low` is usually wasted here — use it
only for subagents inside an already-Opus workflow.

### `medium`
Thinks briefly — enough to organize, not to explore alternatives.
- Summarize a 20-page document or a meeting transcript
- Draft a routine email, docstring, changelog entry
- Review a small, self-contained diff
- Answer a support question with a known answer
- Refactor a function whose shape is well understood
- Research questions where sources are obvious and you just need synthesis

**Opus vs Sonnet:** Sonnet `medium` handles most "office work." Use Opus `medium`
when material is dense or stakes are higher. `medium` matches `high`'s accuracy on
research/knowledge tasks at 70–85% of the cost — solid default for non-coding work
on either model.

### `high` (default)
Safe general-purpose setting. Use whenever unsure.
- Implement a feature spanning two or three files
- Debug a bug that isn't obvious from the stack trace
- "Queue or cron job here?" — design trade-offs
- Multi-source research synthesis with a real conclusion
- Write a technical doc or analysis needing structure and judgment

**Opus vs Sonnet:** Opus `high` is the default for anything intelligence-sensitive.
Sonnet `high` is good value for coding tasks that aren't sprawling — often matches a
previous-generation model at its top effort.

### `xhigh`
Long, tool-heavy sessions where the model must plan, act, check, recover — Claude
Code's default.
- Multi-file change where it edits, runs tests, reads failures, iterates
- Tracking a bug across a large unfamiliar codebase
- A hard algorithmic problem
- Any agentic loop with many steps where a wrong early turn wastes the run

**Opus vs Sonnet:** described as the best setting for most coding/agentic work on
Sonnet 5. On Opus, the step you take when `high` visibly struggles on a long task.
Sonnet `xhigh` is a genuine value play — try it before jumping to Opus.

### `max`
Only when correctness matters more than cost, or you've measured `xhigh` still leaves
quality on the table.
- A concurrency / race-condition bug
- A security-sensitive change
- A migration script that touches production data once
- Math proofs, algorithm design — anywhere a subtle slip invalidates everything
- Deep multi-subtopic research (the one case where *every* effort step keeps adding
  measurable quality)
- A high-stakes one-shot output you won't review line by line

**Opus vs Sonnet:** a `max` task usually needs Opus's ceiling too. Sonnet `max`
costs a lot and can't close a real capability gap.

---

## 6. How to pick, quickly

1. **Mechanical or high-volume?** → `low`, on Sonnet.
2. **Routine work with some judgment?** → `medium`. Step down from `high` here once
   you've spot-checked that quality holds.
3. **Not sure / general coding / analysis?** → `high` on Opus. The honest default.
4. **Long agentic coding session?** → `xhigh`. Try Sonnet `xhigh` first; Opus `xhigh`
   if it stalls.
5. **A mistake here is expensive or unreviewable?** → `max` on Opus.

Two rules of thumb:

- **Before building a "Sonnet-low + escalate to Opus-high" cascade, just try Opus at
  `low` or `medium`.** On the newest models, lower effort often matches a prior setup
  at similar cost with far less plumbing. (Also: caches are per-model, so a cascade
  forfeits cache reuse across its models.)
- **These curves are per-task.** The numbers here are Anthropic's published
  benchmark measurements; the only way to know yours is to run the same 10–20 real
  prompts at two adjacent levels and compare cost, latency, and whether the answers
  actually got better.

---

## 7. Reference facts

- Effort is set via `output_config: {effort: "low"|"medium"|"high"|"xhigh"|"max"}`
  (inside `output_config`, not top-level). GA, no beta header. Default `high`.
- Opus 5 and Sonnet 5 both support all five levels. `xhigh` was added with Opus 4.7.
- Use adaptive thinking (`thinking: {type: "adaptive"}`) alongside effort for the
  best cost/quality trade-off. On Opus 5 thinking is on by default.
- `thinking.display`: `"summarized"` returns a readable reasoning summary;
  `"omitted"` (default on current models) streams empty thinking blocks. Display
  controls visibility only — thinking happens and is billed the same regardless. The
  raw chain of thought is never exposed.
- Changing effort mid-conversation invalidates the prompt cache (except via the
  per-message effort system message on Opus 5 / Fable 5.1). Sweep each effort level
  in a separate session for honest comparison.
- Pricing (as of skill snapshot 2026-06-24): Opus 5 $5 in / $25 out per 1M tokens;
  Sonnet 5 $2 in / $10 out per 1M. Always confirm against current pricing.
- Model-agnostic cost lever order: free wins first (prompt caching, input hygiene,
  loop hygiene, output hygiene, batch), *then* tradeoffs (effort → budgets → model
  choice). Effort is the first quality-trading lever.
