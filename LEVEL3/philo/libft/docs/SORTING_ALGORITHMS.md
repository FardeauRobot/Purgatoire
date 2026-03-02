# Sorting algorithms — mentor notes

## 1) Bubble sort
- **Goal:** walk pairs left→right, swap when out of order until a pass makes zero swaps.
- **Think about:** how do you early-stop when the array is already sorted? One flag is enough.
- **Pitfall:** off-by-one on the inner loop bound as the tail becomes sorted.
- **Debug cue:** count swaps per pass; if it never drops to 0, your bounds are wrong.
- **Schema:**
```
[5 1 4 2]
pass1: [1 4 2 5]
pass2: [1 2 4 5]
pass3: [1 2 4 5]
```

## 2) Selection sort
- **Goal:** for each position i, find the min in i..end, swap into i.
- **Think about:** you only write twice per outer loop (good for low-write media).
- **Pitfall:** forgetting to skip swap when min is already at i (wastes work, breaks stability anyway).
- **Debug cue:** print chosen min index each round—does it walk left→right as expected?
- **Schema:**
```
[4 2 3 1]
step1 → [1 2 3 4]
step2 → [1 2 3 4]
step3 → [1 2 3 4]
```

## 3) Insertion sort
- **Goal:** grow a sorted prefix; insert the new key by shifting bigger elements right.
- **Think about:** loop invariant—left side is always sorted. Use it to reason about bounds.
- **Pitfall:** shifting past index 0; watch your signed/size_t mix.
- **Tip:** use a temp `key` and shift, don’t swap repeatedly.
- **Schema:**
```
[3 1 4 2]
insert 1 → [1 3 4 2]
insert 4 → [1 3 4 2]
insert 2 → [1 2 3 4]
```

## 4) Merge sort (top-down)
- **Goal:** split halves, sort each, merge with a buffer.
- **Think about:** base case when size <= 1; avoid copying small arrays too often.
- **Pitfall:** forgetting to copy back merged buffer into the original array.
- **Tip:** write a `merge(left, mid, right)` helper; test it alone with tiny arrays.
- **Schema:**
```
[4 1 3 2]
split → [4 1] [3 2]
sort →  [1 4] [2 3]
merge → [1 2 3 4]
```

## 5) Quick sort (Hoare/Lomuto)
- **Goal:** pick pivot, partition, recurse on two sides.
- **Think about:** choose a pivot that avoids worst case—maybe middle or median-of-three.
- **Pitfall:** infinite loop if your i/j cross logic is wrong; trace a 3-element case.
- **Tip:** tail-call optimize the larger side to keep stack small.
- **Schema (Hoare-ish):**
```
[4 2 7 1 3], pivot=4
part → [3 2 1 | 4 | 7]
recurse on left/right
```

## 6) Heap sort
- **Goal:** build a max-heap, then pop max to the end, shrinking the heap.
- **Think about:** 0-based heap children at 2i+1, 2i+2; build via bottom-up heapify.
- **Pitfall:** forgetting to reduce heap size after moving max to the end.
- **Debug cue:** after each heapify, check the heap property for a few nodes by hand.
- **Schema:**
```
[4 1 3 2]
heapify → [4 2 3 1]
pop → [1 2 3 |4]
heapify → [3 2 1 |4]
pop → [1 2 |3 4]
```

## 7) Shell sort
- **Goal:** run gapped insertion sorts with shrinking gaps, finishing with gap=1.
- **Think about:** start with a gap like n/2, then n/4 … 1 (or better sequences if you like experiments).
- **Pitfall:** treating gaps as independent passes—remember each pass sees partially sorted data.
- **Tip:** reuse your insertion logic; just step by `gap` instead of 1.
- **Schema (gap 2 then 1):**
```
[8 5 3 7 6 2]
gap2 → [3 2 5 7 6 8]
gap1 → [2 3 5 6 7 8]
```

## 8) Counting sort
- **Goal:** count each value in [min,max], prefix-sum to positions, place into output.
- **Think about:** range size k; if k is huge, pick another sort.
- **Pitfall:** off-by-one in prefix sums shifts everything; test with small min != 0.
- **Tip:** keep it stable by walking input right→left when placing into output.
- **Schema (min=1 max=4):**
```
arr [4 2 1 3 2]
cnt [0 1 2 1 1]
pos [0 1 3 4 5]
out [1 2 2 3 4]
```

## 9) Radix sort (LSD)
- **Goal:** stable counting sort on each digit from least to most significant.
- **Think about:** base choice (10? 256?) — trade buckets vs passes.
- **Pitfall:** forgetting stability between passes breaks the result.
- **Tip:** write a reusable counting-sort-by-digit helper; unit-test a single pass.
- **Schema:**
```
[170 45 75 90 802 24]
1s → [170 90 802 24 45 75]
10s → [802 24 45 75 170 90]
100s → [24 45 75 90 170 802]
```

## 10) Bucket sort
- **Goal:** scatter into buckets (by range), sort each bucket, concatenate.
- **Think about:** bucket count and index function; uniform data likes evenly sized buckets.
- **Pitfall:** empty buckets and one giant bucket—check your distribution.
- **Tip:** use insertion sort inside buckets for simplicity; it’s fast on small lists.
- **Schema (0.0..1.0, 4 buckets):**
```
[0.42 0.32 0.23 0.52 0.25 0.47]
B0: 0.23 0.25
B1: 0.32
B2: 0.42 0.47
B3: 0.52
→ [0.23 0.25 0.32 0.42 0.47 0.52]
```

## General advice
- **Trace tiny arrays (size 3–6) by hand;** if your logic fails there, it’s a bound issue.
- **Assert preconditions in C** (size > 0? pointers not NULL?) to catch misuse early.
- **Measure:** wrap your sorts with a counter for comparisons and swaps to see patterns.
- **After coding a sort**, fuzz it with random arrays and compare output to a trusted `qsort`.
