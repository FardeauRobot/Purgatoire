# 10 Most Useful Functions to Add (Ranked by Difficulty)

Ranked from "Warm-up" to "Boss Fight". Code them in this order to build confidence!

## 1. `ft_strcmp` (Difficulty: 1/10)
*   **Goal:** Compare strings exactly (difference of first non-matching char).
*   **Tip:** Unlike `strncmp`, this goes until `\0`.
*   **Watch out:** Use `unsigned char` for comparison to match standard behavior (avoid negative results for extended ASCII).

## 2. `ft_matrix_len` (Difficulty: 1/10)
*   **Goal:** Count strings in a `NULL`-terminated array (like `argv` or `split` result).
*   **Tip:** It's just `strlen` but iterating a `char **` until `NULL`.
*   **Watch out:** Handle `NULL` input gracefully (return 0).

## 3. `ft_memdel` (Difficulty: 2/10)
*   **Goal:** Free a pointer and set it to `NULL` to prevent double-frees.
*   **Tip:** Pass the *address* of the pointer (`void **ap`) so you can modify the pointer itself.
*   **Watch out:** Check `if (ap && *ap)` before freeing. Set `*ap = NULL` immediately after.

## 4. `ft_strndup` (Difficulty: 3/10)
*   **Goal:** Duplicate at most `n` characters.
*   **Tip:** Useful when parsing "words" inside a larger string without `\0` terminators.
*   **Watch out:** Allocate `len + 1` for the new `\0`. If `s` is shorter than `n`, copy only up to `s`'s end.

## 5. `ft_putnbr_base` (Difficulty: 3/10)
*   **Goal:** Print a number in any base (binary, hex, octal).
*   **Tip:** Recursive approach is cleanest. `base_str` length determines the base.
*   **Watch out:** Handle negative numbers only for base 10 (usually). Recursion base case: `n < base_len`.

## 6. `ft_memswap` (Difficulty: 4/10)
*   **Goal:** Swap the content of ANY two variables (ints, structs, pointers) of the same size.
*   **Discovery:** Generic programming in C using `void *`.
*   **Tip:** Cast to `unsigned char *` to swap byte-by-byte in a loop, OR allocate a temporary buffer.
*   **Watch out:** Ensure both pointers point to valid memory of at least `size` bytes.

## 7. `ft_free_matrix` (Difficulty: 4/10)
*   **Goal:** Free a string array AND the array itself, then set pointer to `NULL`.
*   **Tip:** Take `char ***` so you can set the caller's variable to NULL. Iterate and free sub-strings first.
*   **Watch out:** Free inner strings first, then the outer array. Don't access after freeing.

## 8. `ft_strjoin_free` (Difficulty: 5/10)
*   **Goal:** Join two strings and free one (or both) automatically.
*   **Tip:** Great for loops like `gnl` where you do `str = join(str, buf)`.
*   **Watch out:** Decide if it frees the first, second, or both args. Document it! Usually freeing the first (accumulator) is best.

## 9. `ft_realloc` (Difficulty: 6/10)
*   **Goal:** Resize a memory block while preserving data.
*   **Tip:** `malloc` new size, `ft_memcpy` old data to new, `free` old pointer.
*   **Watch out:** Handle `ptr == NULL` (act like malloc) and `new_size == 0` (act like free). Don't access old memory after freeing!

## 10. `ft_atof` (Difficulty: 7/10)
*   **Goal:** ASCII to float/double (e.g., "-12.34" → -12.34).
*   **Tip:** Parse the integer part like `atoi`. Parse the fractional part by dividing by 10, 100, 1000...
*   **Watch out:** Handle signs (`-`) correctly. Floating point precision issues are normal; `double` is usually enough.
