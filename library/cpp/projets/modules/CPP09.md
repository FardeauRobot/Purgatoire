# 🏗️ CPP09 — The STL in Practice: Pick the Container, Then Defend It

> **TL;DR.** Three real programs: a Bitcoin price lookup (`btc`), a Reverse Polish calculator (`RPN`) and a merge-insertion sort (`PmergeMe`). The twist is a rule about **containers**. You must use at least one per exercise, and **a container used in one exercise can't be used in the ones after it**. PmergeMe needs two. At the defense you're asked *why* you chose each one. So this module is really about the question CPP08 raised: which data structure fits this access pattern?

Related: [`STL.md`](../../notions/advanced/STL.md) · [`ALGORITHMS.md`](../../notions/advanced/ALGORITHMS.md) · [`containers/INDEX.md`](../../notions/containers/INDEX.md) · [`MAP.md`](../../notions/containers/MAP.md) · [`STACK.md`](../../notions/containers/STACK.md) · [`VECTOR.md`](../../notions/containers/VECTOR.md) · [`DEQUE.md`](../../notions/containers/DEQUE.md) · [`LIST.md`](../../notions/containers/LIST.md) · [`STRING.md`](../../notions/fundamentals/STRING.md) · [`TRY_CATCH_THROW.md`](../../lexique/TRY_CATCH_THROW.md) · Previous: [`CPP08.md`](CPP08.md)

---

## 1. The problem, in C you already know

In C, every one of these programs starts by *building* a data structure:
- **btc**: a sorted array of dates plus `bsearch`, or a hand-written BST.
- **RPN**: a fixed `int stack[64]` and a `top` index (push_swap flashbacks).
- **PmergeMe**: `malloc`'d arrays and a lot of `memmove`.

Half the work, and most of the bugs, would be the structure. In C++98 the structures already exist and are tested. What's left is the part that needs thinking: **matching the structure to the job.**

---

## 2. The mechanism: one container plan for the whole module

The rule is: *"You must use at least one container for each exercise… Once a container is used you cannot use it for the rest of the module."* Plan all three exercises **before** writing ex00:

| Exercise | Container | Why this one (your defense answer) |
|---|---|---|
| ex00 `btc` | `std::map<std::string, float>` | Keys stay **sorted**. `upper_bound` finds "the closest earlier date" in O(log n). Dates as `YYYY-MM-DD` strings sort correctly as text |
| ex01 `RPN` | `std::stack<long, std::list<long> >` | RPN *is* a stack: push operands, pop 2 for each operator. Backing it with `list` keeps `deque` free for ex02 (see the gotcha below) |
| ex02 `PmergeMe` | `std::vector<int>` **and** `std::deque<int>` | Both have random access, which binary insertion needs. Comparing a contiguous block with a chunked one is exactly the timing question the evaluator asks |

```
         ex00                     ex01                      ex02
   ┌──────────────┐        ┌──────────────┐        ┌──────────────┬──────────────┐
   │     map      │        │ stack<list>  │        │    vector    │    deque     │
   │ sorted tree  │        │ LIFO         │        │ one block    │ chunks       │
   └──────────────┘        └──────────────┘        └──────────────┴──────────────┘
   map is spent ──────────► list is spent ─────────► vector + deque, both new
```

> ⚠️ **The hidden-container trap.** `std::stack<T>` is backed by a `std::deque<T>` by default. If ex01 uses a plain `std::stack` and ex02 uses `deque`, a strict evaluator can argue you reused `deque`. Naming the underlying container, as in `std::stack<long, std::list<long> >`, removes the argument. Note the space in `> >`: C++98 reads `>>` as the shift operator.

**Makefile rules for every exercise:** `$(NAME)`, `all`, `clean`, `fclean`, `re`, and **no relinking**. `make` a second time must print "nothing to be done".

---

## 3. ex00 — `btc` (Bitcoin Exchange)

### What the subject asks
- `./btc input.txt`
  - The program loads the provided **`data.csv`** (`date,exchange_rate`, one line per day).
  - Then it reads the input file line by line, in the format `date | value`.
  - For each line it prints `date => value = value × rate`.
- **Date:** `Year-Month-Day`, and it must be a real date. **Value:** a float or a positive integer between **0 and 1000**.
- **If the date isn't in the database, use the closest *lower* date.** Never the upper one.

```
$> ./btc
Error: could not open file.
$> ./btc input.txt
2011-01-03 => 3 = 0.9
2011-01-03 => 2 = 0.6
2011-01-03 => 1 = 0.3
2011-01-03 => 1.2 = 0.36
2011-01-09 => 1 = 0.32
Error: not a positive number.
Error: bad input => 2001-42-42
2012-01-11 => 1 = 7.1
Error: too large a number.
```

### The lookup: `upper_bound`, then step back

```
 map (sorted by key):
   2011-01-01  2011-01-03  2011-01-07  2011-01-10 ...
                   ▲           ▲
 query 2011-01-05  │           └── upper_bound: first key  > query
                   └────────────── --it       : last key  <= query  ✓ use this rate

 query 2011-01-03 → upper_bound points at 2011-01-07 → --it → 2011-01-03 (exact match ✓)
 query 2008-12-31 → upper_bound == begin() → there is NO earlier date → error line
```

```cpp
std::map<std::string, float>::const_iterator it = _rates.upper_bound(date);
if (it == _rates.begin())
    throw std::runtime_error("Error: no data before " + date);
--it;
float result = value * it->second;
```

Line by line:
- **`std::map<std::string, float>::const_iterator it`**: an iterator into the map that can only read. Each element it points at is a `std::pair`: `it->first` is the date and `it->second` is the rate.
- **`_rates.upper_bound(date)`**: the map's own member version, O(log n) down the tree. Prefer it over `std::upper_bound`, which can't jump through a tree.
- **`if (it == _rates.begin()) throw …`**: nothing is earlier than the query.
- **`--it;`**: one step back, to the closest date ≤ the query.
- **`value * it->second`**: the line's value times that day's rate.

⚠️ **Where that `throw` is caught matters.** Put the `try`/`catch` **inside** the per-line `while` loop (next section): catch, print the error, go to the next line. If the only `try` is in `main`, the first date before 2009 ends the whole program, and the sheet requires the whole file to be processed.

- **`upper_bound(key)`** returns the first element whose key is **strictly greater**. Stepping back once gives "equal, or the closest lower". This handles an exact match without a special case. `lower_bound` (first key **≥**) would need an extra `if (it->first != date)` check.
- **Check `begin()` before `--it`.** Decrementing `begin()` is undefined behaviour. Any query earlier than the first date in `data.csv` takes this branch.
- **Why string keys work:** `"2011-01-05" < "2011-01-07"` compares character by character. Zero-padded ISO dates sort the same way as the calendar. Once you've validated the format, the string *is* the key.

### Parsing a line

```cpp
std::string line;
std::getline(file, line);                               // skip the header "date | value"
while (std::getline(file, line))
{
    std::string::size_type bar = line.find(" | ");
    if (bar == std::string::npos)
    {
        std::cerr << "Error: bad input => " << line << std::endl;
        continue;
    }
    std::string date = line.substr(0, bar);
    std::string rawValue = line.substr(bar + 3);
    ...
}
```

Line by line:
- **`std::string line;`**: one reusable buffer. `getline` resizes it for you, so there's no `BUFFER_SIZE` to think about.
- **`std::getline(file, line);`** before the loop reads the header. It's `get_next_line` in one call, and it strips the `\n`.
- **`while (std::getline(file, line))`**: `getline` returns the stream, and a stream used as a condition is false at end of file or on error. It's the C++ version of `while ((line = get_next_line(fd)))`.
- **`std::string::size_type bar = line.find(" | ");`**: the index of the separator. `size_type` is the unsigned type `find` returns, so storing it in an `int` would trigger `-Wsign-compare` later.
- **`if (bar == std::string::npos)`**: `npos` is "not found", like `strstr` returning `NULL`. The line is malformed: print it and skip it.
- **`line.substr(0, bar)`**: the characters before the separator, which is the date.
- **`line.substr(bar + 3)`**: everything after it, which is the value. `+ 3` skips the three characters of `" | "`.
- **`...`**: validate the date, parse the value, then do the lookup from the previous section inside a `try`.

- **`continue`, never `return`.** The sheet: *"The program must not stop its execution before having performed the operations on the whole file."* One bad line produces one error line, and processing goes on.
- Convert the value with `std::strtod(raw.c_str(), &end)` and check that `*end == '\0'`. Then check `< 0` (`not a positive number`) and `> 1000` (`too large a number`).
- ⚠️ `strtod` is generous: it also accepts `nan`, `inf`, hex like `0x10`, `1e2` and leading spaces. Tested: `nan` passes **both** range checks (every comparison with NaN is false) and prints garbage. Reject anything that isn't digits with an optional `.` part before calling it.
- **Date validation:** month 1–12, and the day within that month's length, **including leap years**: `(y % 4 == 0 && y % 100 != 0) || y % 400 == 0`. `2001-42-42` fails, and so does `2023-02-29`.

### ⚠️ Gotchas
- **Validate the header line** of the input file, but don't print a result line for it.
- For btc the subject only requires results on stdout, so the error stream is your choice. `std::cerr` is the conventional one, and the subject's example only looks mixed because both streams share the terminal.
- **Empty file / file with only a header:** print nothing and don't crash. The sheet tests an empty file.
- `data.csv` is part of the project. Load it with a path relative to where the program runs, and handle it being missing with a clean error.

---

## 4. ex01 — `RPN` (Reverse Polish Notation)

### What the subject asks
- `./RPN "<expression>"`. Operands are single digits **< 10** (results can be bigger). Operators are `+ - * /`.
- Print the result on stdout, or `Error` on stderr.
- Brackets and decimal numbers don't need to be handled.

### The whole algorithm is one stack

```
 "8 9 * 9 - 9 - 9 - 4 - 1 +"

 token  action              stack (bottom → top)
 8      push                [8]
 9      push                [8 9]
 *      pop 9, pop 8, 8*9   [72]
 9      push                [72 9]
 -      72-9                [63]
 9 -                        [54]
 9 -                        [45]
 4 -                        [41]
 1 +                        [42]
 end    exactly one left →  42 ✓
```

**Operand order matters:** the *first* pop is the **right-hand side**. `"5 3 -"` means `5 - 3`, not `3 - 5`.

### Core snippet

```cpp
static long evaluate(std::string const &expr)
{
    std::stack<long, std::list<long> >  st;
    std::istringstream                  in(expr);
    std::string                         tok;

    while (in >> tok)
    {
        if (tok.size() == 1 && std::isdigit(tok[0]))
            st.push(tok[0] - '0');
        else if (tok.size() == 1 && std::string("+-*/").find(tok[0]) != std::string::npos)
        {
            if (st.size() < 2)
                throw std::runtime_error("not enough operands");
            long right = st.top();
            st.pop();
            long left = st.top();
            st.pop();
            if (tok[0] == '+')
                st.push(left + right);
            else if (tok[0] == '-')
                st.push(left - right);
            else if (tok[0] == '*')
                st.push(left * right);
            else
            {
                if (right == 0)
                    throw std::runtime_error("division by zero");
                st.push(left / right);
            }
        }
        else
            throw std::runtime_error("invalid token: " + tok);
    }
    if (st.size() != 1)
        throw std::runtime_error("leftover operands");
    return st.top();
}
```

Line by line:
- **`std::stack<long, std::list<long> > st;`**: the stack, with its backing container named explicitly (see the hidden-container trap in §2).
- **`std::istringstream in(expr);`**: wraps the string in a stream so you can read from it with `>>`, just as you read from `std::cin`.
- **`while (in >> tok)`**: each `>>` skips spaces and reads one word. The loop ends when the words run out.
- **`st.push(tok[0] - '0');`**: `'7' - '0'` is 7, the same char-to-digit trick as `ft_atoi`.
- **`std::string("+-*/").find(tok[0]) != std::string::npos`**: "is this character one of the four operators?", which is `ft_strchr("+-*/", c)` in C.
- **`long right = st.top(); st.pop();`**: `top()` reads and `pop()` removes. They're two separate calls because `pop()` returns `void` in the STL.
- **`long left = st.top(); st.pop();`**: the second pop is the **left** operand.
- **The `if` / `else if` chain** computes `left op right` and pushes the result back. The result becomes an operand for the next operator.
- **`if (right == 0)`** is checked *before* dividing. Integer division by zero is undefined behaviour. On x86 Linux, which most 42 clusters run, it kills the process with `SIGFPE`, and a crash during the defense is a 0. On your ARM Mac it silently gives `0` (tested: `7 / 0` printed `0`), so testing only at home would hide the bug.
- **`else throw … "invalid token"`**: any other word, like `(` or `12`.
- **`if (st.size() != 1)`**: a valid expression reduces to exactly one value. `"1 2"` leaves two.
- **`return st.top();`**: that value is the result.

Walkthrough:
- **`std::istringstream in(expr); in >> tok`** splits on whitespace for you. It's `ft_split(expr, ' ')` without the memory management.
- **`tok.size() == 1`** makes `"12"` an error (operands must be < 10), and it rejects `"("` through the `else` branch.
- **Three error cases, each a `throw`:** fewer than 2 operands for an operator, division by zero, and not exactly one value left at the end. `main` catches `std::exception &` and prints `Error` to stderr, the CPP05 pattern.
- **`long` instead of `int`** delays overflow: `"9 9 * 9 * 9 * …"` grows fast. The subject doesn't require overflow detection, but you should know where your limit is.

### Tested results

| Expression | Output |
|---|---|
| `8 9 * 9 - 9 - 9 - 4 - 1 +` (subject) | `42` |
| `7 7 * 7 -` (subject) | `42` |
| `1 2 * 2 / 2 * 2 4 - +` (subject) | `0` |
| `(1 + 1)` (subject) | `Error` |
| `9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -` (grading sheet) | `42` |
| `1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /` (grading sheet) | `15` |
| `1 0 /` · `1 +` · `1 2` · `12 3 +` | `Error` |

### ⚠️ Gotchas
- Integer division truncates toward zero: `"7 2 /"` gives `3`. That's fine; decimals aren't required.
- Include `<cctype>` for `isdigit`, `<sstream>` for `istringstream` and `<list>` for the stack's backing container.

---

## 5. ex02 — `PmergeMe` (Ford-Johnson merge-insertion sort)

### What the subject asks
- `./PmergeMe 3 5 9 7 4`: positive integers as arguments. Anything else, like `"-1"`, is `Error` on stderr.
- Sort them with the **merge-insertion sort** (Ford-Johnson, Knuth's *TAOCP* Vol. 3, p. 184), **once per container**. The subject advises a separate implementation for each container, rather than one generic function.
- It must handle **at least 3000** integers. Handling duplicates is up to you.
- Output:

```
$> ./PmergeMe 3 5 9 7 4
Before: 3 5 9 7 4
After: 3 4 5 7 9
Time to process a range of 5 elements with std::[..] : 0.00031 us
Time to process a range of 5 elements with std::[..] : 0.00014 us
$> ./PmergeMe "-1" "2"
Error
```

- Four lines: before, after, then one time line per container that **names the container** (fill in `std::[..]`).
- **The time format is up to you**, but the precision must make the difference between the two containers visible. The subject's own numbers are "deliberately strange".
- The time covers **everything the container does**: parsing into it *and* sorting it.

### Why this algorithm exists
Ford-Johnson isn't the *fastest* sort in practice. It's designed to use **as few comparisons as possible**, and for small n it's essentially optimal. Our tested implementation (below) never used more comparisons than the theoretical minimum worst case, F(n) = Σ⌈log₂(3k/4)⌉ (13 for n=7, 66 for n=21), over 3000 random inputs for every n from 1 to 33. That's the idea to explain at the defense: **it saves comparisons, not time.**

### The three steps

```
 input:  8 3 6 1 7 2 5                               (n = 7)

 ① PAIR and compare inside each pair (⌊n/2⌋ comparisons)
         (8,3) (6,1) (7,2)   straggler: 5
          a b   a b   a b                a = larger ("winner"), b = smaller

 ② RECURSIVELY sort the winners with this same algorithm
         a:  6  7  8     → rename in sorted order: a1=6 a2=7 a3=8
         b:  1  2  3                               b1=1 b2=2 b3=3   b4=5 (straggler)

    main chain = b1 a1 a2 a3 = 1 6 7 8    (b1 ≤ a1 ≤ everything, so it goes in front for free)

 ③ INSERT the pending b's in JACOBSTHAL order, each one with binary search,
    searching ONLY to the left of its own partner a_k (since b_k ≤ a_k)

    order: b3 b2 | b5 b4 | b11 b10 … b6 | b21 … b12 | …    (cut at the number of b's)

    b3=3: search [1 6 7]   (left of a3=8)   → 1 3 6 7 8
    b2=2: search [1 3 6]   (left of a2=7)   → 1 2 3 6 7 8
    b4=5: straggler, no partner → whole chain → 1 2 3 5 6 7 8   ✓
```

### Why Jacobsthal order?
Binary search on a range of **2ᵏ − 1** elements takes exactly *k* comparisons. On 2ᵏ elements it takes *k + 1*. The Jacobsthal numbers **1, 3, 5, 11, 21, 43…** (`J(n) = J(n−1) + 2·J(n−2)`) are chosen so that each group's search area stays at 2ᵏ − 1 elements. Inserting the *highest* index of a group first means the elements inserted afterwards don't grow the ranges that still have to be searched. Insert in plain order `b2 b3 b4…` and the algorithm still sorts, but it isn't Ford-Johnson anymore. Be ready to explain this at the defense.

```cpp
static std::vector<size_t> jacobsthalOrder(size_t m)
{
    std::vector<size_t> order;
    size_t prev = 1;
    size_t a = 1;
    size_t b = 3;

    while (prev < m)
    {
        size_t hi = std::min(b, m);
        for (size_t k = hi; k > prev; --k)
            order.push_back(k);
        prev = hi;
        size_t next = b + 2 * a;
        a = b;
        b = next;
    }
    return order;
}
```

`jacobsthalOrder(12)` gives `b3 b2 b5 b4 b11 b10 b9 b8 b7 b6 b12`. `m` counts all the b's (the straggler included), and `b1` is skipped because it's already in the chain.

Line by line:
- **`std::vector<size_t> order;`**: the result, a list of b indices in the order to insert them.
- **`prev = 1`**: the highest index already handled. `b1` is handled for free, so we start at 1.
- **`a = 1`, `b = 3`**: two consecutive Jacobsthal numbers. `b` is the top of the next group.
- **`while (prev < m)`**: stop once every b up to `m` has been scheduled.
- **`hi = std::min(b, m)`**: the group's top, clipped to `m`. The last group is usually incomplete.
- **`for (k = hi; k > prev; --k) order.push_back(k);`**: the group **counts down** from `hi` to `prev + 1`. That's the "highest index first" rule.
- **`prev = hi;`**: this group is done.
- **`next = b + 2 * a; a = b; b = next;`**: moves one step along J(n) = J(n−1) + 2·J(n−2), like advancing a Fibonacci pair.

Each turn of the loop for `m = 12` (printed by a test run):

| prev | a | b | hi | pushed |
|---|---|---|---|---|
| 1 | 1 | 3 | 3 | 3 2 |
| 3 | 3 | 5 | 5 | 5 4 |
| 5 | 5 | 11 | 11 | 11 10 9 8 7 6 |
| 11 | 11 | 21 | 12 | 12 |

On the n = 7 example there are four b's (b1 b2 b3 plus the straggler b4), so `jacobsthalOrder(4)` gives `3 2 4`.

### Step ③ in code: the bounded binary insertion

The loop below uses the state built by steps ① and ②. Here it is for the n = 7 example:

| Name | Type | Contents for `8 3 6 1 7 2 5` | Meaning |
|---|---|---|---|
| `bigs` | `std::vector<int>` | `6 7 8` | the winners, sorted by step ②. `bigs[k-1]` is `a_k` |
| `smalls` | `std::vector<int>` | `1 2 3` | each winner's partner, same order. `smalls[k-1]` is `b_k` |
| `straggler` | `int` | `5` | the unpaired last element (only when n is odd) |
| `chain` | `std::vector<int>` | `1 6 7 8` | `b1` followed by all the `a`s: the growing sorted result |
| `aPos` | `std::vector<size_t>` | `[-] 1 2 3` | `aPos[k]` = the current index of `a_k` in `chain`. Slot 0 is unused |
| `order` | `std::vector<size_t>` | `3 2 4` | `jacobsthalOrder(smalls.size() + 1)`, the `+ 1` counting the straggler |

⚠️ **Watch the mixed indexing.** `k`, `order` and `aPos` count from **1**, like the maths (`b1`, `a1`). `smalls` and `bigs` are ordinary vectors counting from **0**, hence `smalls[k - 1]`. Mix them up and you get an off-by-one that still *sorts*, but searches the wrong range.

```cpp
for (size_t o = 0; o < order.size(); o++)
{
    size_t k = order[o];
    int    value;
    size_t bound;
    if (k > bigs.size())
    {
        value = straggler;
        bound = chain.size();
    }
    else
    {
        value = smalls[k - 1];
        bound = aPos[k];
    }
    std::vector<int>::iterator pos =
        std::upper_bound(chain.begin(), chain.begin() + bound, value);
    size_t q = pos - chain.begin();
    chain.insert(pos, value);
    for (size_t j = 1; j <= bigs.size(); j++)
        if (aPos[j] >= q)
            aPos[j]++;
}
```

Line by line:
- **`size_t k = order[o];`**: which `b` to insert this time.
- **`if (k > bigs.size())`**: an index past the last pair can only be the straggler. It has no partner, so its search `bound` is the **whole** chain.
- **`else { value = smalls[k - 1]; bound = aPos[k]; }`**: a normal `b_k`. Its search stops at `a_k`'s current position, because `b_k ≤ a_k` is already known.
- **`std::upper_bound(chain.begin(), chain.begin() + bound, value)`**: binary search in `[0, bound)`. It returns an iterator to the first element greater than `value`.
- **`size_t q = pos - chain.begin();`**: iterator minus `begin()` gives an index, the same pointer arithmetic as `ptr - arr` in C.
- **`chain.insert(pos, value);`**: puts the value at index `q` and shifts everything after it one step right.
- **The inner `for`**: every `a` that was at index `q` or later has just moved right by one, so its `aPos` is bumped.

Traced on the n = 7 example (output of a test run):

| Step | Value | Search range | `q` | `chain` after | `aPos[1..3]` after |
|---|---|---|---|---|---|
| start | | | | `1 6 7 8` | 1 2 3 |
| b3 | 3 | `[1 6 7]` (bound 3) | 1 | `1 3 6 7 8` | 2 3 4 |
| b2 | 2 | `[1 3 6]` (bound 3) | 1 | `1 2 3 6 7 8` | 3 4 5 |
| b4 | 5 (straggler) | whole chain (bound 6) | 3 | `1 2 3 5 6 7 8` | 4 5 6 |

Look at b2: without the `aPos` update, its bound would still be 2 (a2's *original* place) and the search range would be `[1 3]`, which stops short of `6`. With b2 = 2 that's harmless by luck. Change the input so that b2 is bigger than a1 (pairs `(6,1) (8,7) (9,3)`) and the stale bound inserts 7 *before* 6. Tested: the output is `1 3 7 6 8 9`, **not sorted**. With the update it's `1 3 6 7 8 9`.

- **`std::upper_bound` *is* the binary search.** Don't write your own. Its search range stops at `chain.begin() + bound`, which is the position of `a_k`.
- **`aPos[k]`** records where `a_k` currently sits in the chain. Every insertion at position `q` shifts the a's at or after `q` one step right, so their positions are updated. Without this bookkeeping the bounds go stale. Positions only ever grow, so a stale bound always stops **short** of `a_k`, and values end up in the wrong place (see the trace above). If you give up on bounds and search the whole chain instead, it sorts correctly but you lose the comparison savings.
- **`upper_bound`, not `lower_bound`,** so equal values go after existing ones. Duplicates are allowed and still sort correctly (tested).
- **The pairing step when there are duplicates:** after the recursive sort of the winners, each sorted `a` has to find its own `b` again. Keep the pairs (e.g. `std::vector<std::pair<int,int> >`) and match each sorted winner to an unused pair with the same value. Equal winners can swap partners safely, because the partner is still ≤ its winner.

### Timing (C++98: no `<chrono>`)

```cpp
std::clock_t start = std::clock();
std::vector<int> v = parseInto<std::vector<int> >(ac, av);
fordJohnsonVector(v);
std::clock_t end = std::clock();
double us = 1000000.0 * (end - start) / CLOCKS_PER_SEC;
```

`std::clock()` from `<ctime>` measures processor time. Convert it to microseconds, as the subject's example does.

Line by line:
- **`std::clock_t start = std::clock();`**: a timestamp taken *before* parsing, because the subject counts parsing as part of the work.
- **`parseInto<std::vector<int> >(ac, av)`**: a helper you write, a function template that fills any container from `argv`. The explicit `<std::vector<int> >` is needed because the container type appears only in the return value, which the compiler can't deduce from.
- **`fordJohnsonVector(v);`**: the sort for that container. Write a second one for the `deque`.
- **`std::clock_t end = std::clock();`**: the second timestamp.
- **`1000000.0 * (end - start) / CLOCKS_PER_SEC`**: ticks → seconds → microseconds. The `1000000.0` is a `double` and goes **first**, so everything after it is computed in `double`. Otherwise an integer division by `CLOCKS_PER_SEC` would round a short run down to `0`.

**Measured on this Mac** (3000 random ints in 1–1000, while the subject's command uses 1–100000; no optimisation, same implementation for both containers): **vector ≈ 28 ms, deque ≈ 31 ms**, consistent across runs. This is how to explain the gap:

| | `std::vector` | `std::deque` |
|---|---|---|
| Memory | One contiguous block | Fixed-size chunks + a map of chunk pointers |
| `chain.begin() + bound`, `*it` | Plain pointer arithmetic | Two indirections (chunk table, then chunk) |
| `insert` in the middle | `memmove`-like shift of the tail | Shifts toward the nearer end, across chunks |
| CPU cache | Excellent: neighbours are adjacent | Worse: jumps between chunks |

Binary search and middle insertion are exactly the operations where a contiguous block wins. Your numbers will differ. What matters is **explaining** them.

### Parsing arguments

```cpp
for (int i = 1; i < ac; i++)
{
    char *end;
    errno = 0;
    long n = std::strtol(av[i], &end, 10);
    if (*av[i] == '\0' || *end != '\0' || errno == ERANGE || n <= 0 || n > INT_MAX)
        throw std::runtime_error("Error");
    v.push_back(static_cast<int>(n));
}
```

Line by line:
- **`for (int i = 1; …)`**: from 1, because `av[0]` is the program name.
- **`char *end;`**: `strtol` sets it to the first character it could *not* read.
- **`errno = 0;`**: `strtol` only *sets* `errno` on overflow and never clears it, so reset it before each call.
- **`long n = std::strtol(av[i], &end, 10);`**: base 10. It's `ft_atoi` with error reporting.
- **The five-part condition:** `*av[i] == '\0'` is an empty argument; `*end != '\0'` means junk after the digits (`3a`); `errno == ERANGE` means too big for a `long`; `n <= 0` rejects negatives and zero; `n > INT_MAX` means it won't fit the `int` we store.
- **`v.push_back(static_cast<int>(n));`**: `long` → `int` is narrowing, so the cast is explicit (CPP06), and safe because of the range check just above.

This needs `<cerrno>` and `<climits>`. It rejects `-1`, `abc`, `3a`, empty strings and overflow, but `strtol` still accepts leading spaces and `+`, so `" +5"` gets through. Whether `0` counts as "positive" is your call: the check above rejects it. Say so at the defense.

### ⚠️ Gotchas
- **Evaluators check that the algorithm is present for *each* container.** Using `std::sort` on the second one to "check" the result is fine in a test. As the implementation, it isn't.
- Both containers must come out **sorted identically**. Compare them in debug builds.
- The test command on macOS is `./PmergeMe $(jot -r 3000 1 100000 | tr '\n' ' ')`. On Linux it's `shuf -i 1-100000 -n 3000 | tr "\n" " "`. `jot -r` draws with replacement, so even that range **can contain duplicates** (38 in a test run here), so decide how you handle them *before* the defense.
- The sheet also tests 5–10 hand-picked numbers first. If that small test fails, the evaluation stops.

---

## 6. What the evaluator checks

From the official 42evalhub grading sheet:

> The subject's last chapter adds that the defense **may ask for a small live modification** (change a display, adjust a data structure). Knowing your own code line by line is part of the grade.

**Prerequisites**
- [ ] `c++ -Wall -Wextra -Werror`, C++98. Containers are allowed, since the module is about the STL.
- [ ] No function body in a header (templates excepted). The Makefile uses `c++` and the flags.
- [ ] No `*alloc`, `*printf` or `free`. No external library.
- [ ] No leaks. **No crash or unexpected termination during the whole defense**, otherwise the grade is 0.

**ex00: btc**
- [ ] A Makefile with the usual rules. At least one container, and you can **explain why that one**.
- [ ] Error handling: an empty file, a file with errors, wrong dates, values > 1000 or < 0. **The whole file is processed.**
- [ ] With the evaluator's `input.csv` (which they may edit), results match manual checks, and a missing date uses the **nearest lower date**.

**ex01: RPN**
- [ ] Makefile. A container, with justification. **Not a container from ex00.**
- [ ] Correct results on the evaluator's own formulas, including the two extra ones in §4. No parentheses or decimals required.

**ex02: PmergeMe**
- [ ] Makefile. **Two containers**, with justification. **Neither one used in ex00 or ex01.**
- [ ] Merge-insertion (Ford-Johnson) is **present and used for each container**. You give a short explanation. *"In case of doubt, the evaluation stops here."*
- [ ] 5–10 hand-picked positive integers come out sorted.
- [ ] 3000 random integers (`jot` / `shuf`) work, and you can **explain the timing difference** between the containers.

---

## 7. Cheat sheet

| Need | C++98 spelling |
|---|---|
| Closest key ≤ `k` | `it = m.upper_bound(k); if (it != m.begin()) --it;` |
| Read a file line by line | `std::ifstream f(path); while (std::getline(f, line))` |
| Split on whitespace | `std::istringstream in(s); while (in >> tok)` |
| String → number, strictly | `strtod` / `strtol` with an `end` pointer, `*end == '\0'`, `errno` |
| Stack with a chosen backing container | `std::stack<long, std::list<long> >` (space in `> >`) |
| Binary search for an insertion point | `std::upper_bound(first, last, value)` |
| Insert at an iterator | `c.insert(pos, value)` |
| CPU time | `std::clock()` / `CLOCKS_PER_SEC` from `<ctime>` |
| Leap year | `(y % 4 == 0 && y % 100 != 0) \|\| y % 400 == 0` |

**What this module leaves you with:** the question "which container?" now has an answer that depends on the *access pattern*: sorted lookup → `map`, LIFO → `stack`, random access with cache-friendly inserts → `vector`. That reflex is what the whole C++ piscine was building toward, and it carries straight into webserv.

➡️ Next: [`../webserv/INDEX.md`](../webserv/INDEX.md). The STL and RAII meet sockets and `poll()`.
