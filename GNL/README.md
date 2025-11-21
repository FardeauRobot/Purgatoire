# Libft 🔧

A compact C utility library aligned with the 42 curriculum. `libft` exposes safe, familiar helpers from <ctype.h>, <string.h>, and <stdlib.h>. All routines live in `libft.h` and compile into the static `libft.a` archive so you can reuse them across projects without dragging in an entire system header set.

Each code block below uses a fixed-width layout so every line has the same number of characters, making the reference easy to scan in raw form.

## Function reference

### Character classification & case conversion
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_isalnum       | int (int)                                          | Truthy for ASCII letters or digits.                          |
| ft_isalpha       | int (int)                                          | Truthy for ASCII letters only.                               |
| ft_isascii       | int (int)                                          | Limits checks to the 7-bit ASCII range.                      |
| ft_isdigit       | int (int)                                          | Truthy for ASCII digits '0'..'9'.                            |
| ft_isprint       | int (int)                                          | Truthy for printable chars (space incl).                     |
| ft_tolower       | int (int)                                          | Downcases ASCII letters.                                     |
| ft_toupper       | int (int)                                          | Upcases ASCII letters.                                       |
```

### String utilities
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_strlen        | size_t (const char *)                              | Returns string length (excl. NUL).                           |
| ft_strdup        | char * (const char *)                              | Allocates and copies the string.                             |
| ft_strchr        | char * (const char *, int)                         | Finds the first occurrence of c.                             |
| ft_strrchr       | char * (const char *, int)                         | Finds the last occurrence of c.                              |
| ft_strncmp       | int (const char *, const char *, size_t)           | Compares up to n bytes.                                      |
| ft_strnstr       | char * (const char *, const char *, size_t)        | Searches needle within len bytes.                            |
| ft_strlcpy       | size_t (char *, const char *, size_t)              | Safely copies, always NUL-terminates.                        |
| ft_strlcat       | size_t (char *, const char *, size_t)              | Appends, respecting dstsize.                                 |
| ft_substr        | char * (char const *, unsigned int, size_t)        | Allocates a substring.                                       |
| ft_strjoin       | char * (char const *, char const *)                | Concatenates two strings.                                    |
| ft_strtrim       | char * (char const *, char const *)                | Trims set chars from both ends.                              |
| ft_split         | char ** (char const *, char)                       | Splits s on delimiter c.                                     |
| ft_strmapi       | char * (char const *, char (*f)(unsigned int, char)) | Maps f over each char with index.                            |
| ft_striteri      | void (char *, void (*f)(unsigned int, char *))     | Calls f in-place on each char.                               |
```

### Memory management
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
```

### Conversion utilities
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_atoi          | int (const char *)                                 | Parses as signed decimal integer.                            |
| ft_itoa          | char * (int)                                       | Converts int to decimal string.                              |
```

### File descriptor output helpers
```text
| Function         | Signature                                          | Description                                                  |
| ---------------- | -------------------------------------------------- | ------------------------------------------------------------ |
| ft_putchar_fd    | void (char, int)                                   | Writes char to fd.                                           |
| ft_putstr_fd     | void (char *, int)                                 | Writes string to fd.                                         |
| ft_putendl_fd    | void (char *, int)                                 | Writes string + newline to fd.                               |
| ft_putnbr_fd     | void (int, int)                                    | Writes decimal digits to fd.                                 |
```

### Linked list helpers (bonus)
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
```

## Building & usage

Use the provided Makefile with `cc -Wall -Wextra -Werror`:

```sh
make        # build mandatory helpers into libft.a
make bonus  # build bonus linked-list helpers as well
make clean  # remove *.o files
make fclean # delete *.o files and libft.a
make re     # fclean + make
```

> 💡 Link `libft.a` alongside your sources and include `libft.h` so the compiler resolves the helper declarations:

```sh
cc main.c libft.a -I. -o my_program
```
