# Libft 🔧

A compact C utility library aligned with the 42 curriculum. `libft` exposes safe, familiar helpers from <ctype.h>, <string.h>, and <stdlib.h>. All routines live in `libft.h` and compile into the static `libft.a` archive so you can reuse them across projects without dragging in an entire system header set.

## Project Structure

The source files are organized into the following subdirectories:

- **includes/**: All header files.
- **char/**: Character checks and case conversion (`ft_isalnum`, `ft_toupper`, etc.)
- **conversion/**: String/Integer conversions (`ft_atoi`, `ft_itoa`, etc.)
- **gc/**: Garbage Collector and GC-managed functions (`ft_gc_malloc`, `ft_calloc_gc`, etc.)
- **gnl/**: Get Next Line (`get_next_line`)
- **list/**: Linked list manipulation (`ft_lstnew`, `ft_lstadd_back`, etc.)
- **memory/**: Memory manipulation (`ft_memset`, `ft_calloc`, etc.)
- **output/**: Printing and file descriptor output (`ft_putstr_fd`, `ft_printf`, etc.)
- **sorting/**: Sorting algorithms.
- **strings/**: String manipulation (`ft_strlen`, `ft_split`, etc.)

Each code block below uses a fixed-width layout so every line has the same number of characters, making the reference easy to scan in raw form.

## Function reference

### Character classification & case conversion (Char/)
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_isalnum       | int (int)                                          | Truthy for ASCII letters or digits.                          |
| ft_isalpha       | int (int)                                          | Truthy for ASCII letters only.                               |
| ft_isascii       | int (int)                                          | Limits checks to the 7-bit ASCII range.                      |
| ft_ischarset     | int (int, char *)                                   | Truthy when the character appears inside `charset`.          |
| ft_isdigit       | int (int)                                          | Truthy for ASCII digits '0'..'9'.                            |
| ft_isprint       | int (int)                                          | Truthy for printable chars (space incl).                     |
| ft_isspace       | int (int)                                          | Detects ASCII whitespace (space or control 9..13).           |
| ft_is_only       | int (const char *, int (*)(int))                   | Returns 1 only if every char in `s` satisfies the predicate. |
| ft_tolower       | int (int)                                          | Downcases ASCII letters.                                     |
| ft_toupper       | int (int)                                          | Upcases ASCII letters.                                       |
```

### String utilities (Strings/)
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_strlen        | size_t (const char *)                              | Returns string length (excl. NUL).                           |
| ft_strdup        | char * (const char *)                              | Allocates and copies the string.                             |
| ft_strchr        | char * (const char *, int)                         | Finds the first occurrence of c.                             |
| ft_strcheck      | int (const char *, int (*)(int))                   | Verifies every character passes `check`.                     |
| ft_strrchr       | char * (const char *, int)                         | Finds the last occurrence of c.                              |
| ft_strncmp       | int (const char *, const char *, size_t)           | Compares up to n bytes.                                      |
| ft_strnstr       | char * (const char *, const char *, size_t)        | Searches needle within len bytes.                            |
| ft_strlcpy       | size_t (char *, const char *, size_t)              | Safely copies, always NUL-terminates.                        |
| ft_strlcat       | size_t (char *, const char *, size_t)              | Appends, respecting dstsize.                                 |
| ft_substr        | char * (char const *, unsigned int, size_t)        | Allocates a substring.                                       |
| ft_strjoin       | char * (char const *, char const *)                | Concatenates two strings.                                    |
| ft_strtrim       | char * (char const *, char const *)                | Trims set chars from both ends.                              |
| ft_split_charset | char ** (char const *, char *)                     | Splits `s` using any char from `charset` as delimiters.      |
| ft_split_sep     | char ** (char const *, char)                       | Splits `s` on a single delimiter char, skipping repeats.     |
| ft_strmapi       | char * (char const *, char (*f)(unsigned int, char)) | Maps f over each char with index.                            |
| ft_striteri      | void (char *, void (*f)(unsigned int, char *))     | Calls f in-place on each char.                               |
```

### Memory management (Memory/)
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_memset        | void * (void *, int, size_t)                       | Fills len bytes with c.                                      |
| ft_bzero         | void (void *, size_t)                              | Zeroes n bytes.                                              |
| ft_memcpy        | void * (void *, const void *, size_t)              | Copies n bytes (no overlap).                                 |
| ft_memmove       | void * (void *, const void *, size_t)              | Copies n bytes safely (overlap OK).                          |
| ft_memchr        | void * (const void *, int, size_t)                 | Searches first n bytes for c.                                |
| ft_memcmp        | int (const void *, const void *, size_t)           | Compares first n bytes.                                      |
| ft_calloc        | void * (size_t, size_t)                            | Allocates & zeroes count * size bytes.                       |
| ft_free_array    | void (char **)                                     | Frees a NULL-terminated array of strings.                    |
```

### Conversion utilities (Conversion/)
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_atoi          | int (const char *)                                 | Parses as signed decimal integer.                            |
| ft_atol          | long (const char *)                                | Parses as signed decimal long integer.                       |
| ft_itoa          | char * (int)                                       | Converts int to decimal string.                              |
| ft_overint       | int (const char *)                                 | Checks for integer overflow.                                 |
```

### File descriptor output helpers (Output/)
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_putchar_fd    | void (char, int)                                   | Writes char to fd.                                           |
| ft_putstr_fd     | void (char *, int)                                 | Writes string to fd.                                         |
| ft_putendl_fd    | void (char *, int)                                 | Writes string + newline to fd.                               |
| ft_putnbr_fd     | void (int, int)                                    | Writes decimal digits to fd.                                 |
| ft_printf        | int (const char *, ...)                            | Formatted output conversion.                                 |
```

### Linked list helpers (List/)
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_lstnew        | t_list * (void *)                                  | Creates node with content.                                   |
| ft_lstsize       | int (t_list *)                                     | Counts nodes.                                                |
| ft_lstlast       | t_list * (t_list *)                                | Returns last node or NULL.                                   |
| ft_lstadd_front  | void (t_list **, t_list *)                         | Prepends node to list.                                       |
| ft_lstadd_back   | void (t_list **, t_list *)                         | Appends node to list.                                        |
| ft_lstdelone     | void (t_list *, void (*del)(void *))               | Frees node using del.                                        |
| ft_lstclear      | void (t_list **, void (*del)(void *))              | Clears entire list, NULLs pointer.                           |
| ft_lstiter       | void (t_list *, void (*f)(void *))                 | Runs f on each node content.                                 |
| ft_lstmap        | t_list * (t_list *, void *(*f)(void *), void (*del)(void *)) | Maps f, builds new list.                                     |
| ft_lstprint_nbr  | void (t_list *)                                    | Prints list content as integers.                             |
```

### Get Next Line (GNL/)
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| get_next_line    | char * (int)                                       | Reads a line from a file descriptor.                         |
```

## Building & usage

Use the provided Makefile with `cc -Wall -Wextra -Werror`:

```sh
make        # build all helpers into libft.a
make clean  # remove *.o files
make fclean # delete *.o files and libft.a
make re     # fclean + make
```

> 💡 Link `libft.a` alongside your sources and include `libft.h` so the compiler resolves the helper declarations:

```sh
cc main.c libft.a -I. -o my_program
```
