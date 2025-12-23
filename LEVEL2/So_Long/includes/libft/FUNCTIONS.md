# Libft - Detailed Function Reference

This document provides comprehensive details for each function in the `libft` library, including prototypes, parameters, return values, and behavior notes.

## Table of Contents

### Character Classification & Case Conversion (Char/)
- [`ft_isalnum`](#ft_isalnum) - Tests for alphanumeric character
- [`ft_isalpha`](#ft_isalpha) - Tests for alphabetic character
- [`ft_isascii`](#ft_isascii) - Tests for ASCII character
- [`ft_isdigit`](#ft_isdigit) - Tests for decimal digit
- [`ft_isprint`](#ft_isprint) - Tests for printable character
- [`ft_tolower`](#ft_tolower) - Converts to lowercase
- [`ft_toupper`](#ft_toupper) - Converts to uppercase

### String Utilities (Strings/)
- [`ft_strlen`](#ft_strlen) - Calculate string length
- [`ft_strdup`](#ft_strdup) - Duplicate a string
- [`ft_strchr`](#ft_strchr) - Locate first occurrence of character
- [`ft_strrchr`](#ft_strrchr) - Locate last occurrence of character
- [`ft_strncmp`](#ft_strncmp) - Compare strings up to n bytes
- [`ft_strnstr`](#ft_strnstr) - Locate substring in string
- [`ft_strlcpy`](#ft_strlcpy) - Size-bounded string copy
- [`ft_strlcat`](#ft_strlcat) - Size-bounded string concatenation
- [`ft_substr`](#ft_substr) - Extract substring
- [`ft_strjoin`](#ft_strjoin) - Concatenate two strings
- [`ft_strtrim`](#ft_strtrim) - Trim characters from string
- [`ft_split`](#ft_split) - Split string by delimiter
- [`ft_strmapi`](#ft_strmapi) - Apply function to each character
- [`ft_striteri`](#ft_striteri) - Apply function to each character with index

### Memory Management (Memory/)
- [`ft_memset`](#ft_memset) - Fill memory with constant byte
- [`ft_bzero`](#ft_bzero) - Zero a byte string
- [`ft_memcpy`](#ft_memcpy) - Copy memory area
- [`ft_memmove`](#ft_memmove) - Copy memory area (handles overlap)
- [`ft_memchr`](#ft_memchr) - Scan memory for character
- [`ft_memcmp`](#ft_memcmp) - Compare memory areas
- [`ft_calloc`](#ft_calloc) - Allocate and zero memory
- [`ft_free_array`](#ft_free_array) - Free array of strings

### Conversion Utilities (Conversion/)
- [`ft_atoi`](#ft_atoi) - Convert string to integer
- [`ft_atol`](#ft_atol) - Convert string to long integer
- [`ft_itoa`](#ft_itoa) - Convert integer to string
- [`ft_overint`](#ft_overint) - Check for integer overflow

### File Descriptor Output (Output/)
- [`ft_putchar_fd`](#ft_putchar_fd) - Output character to file descriptor
- [`ft_putstr_fd`](#ft_putstr_fd) - Output string to file descriptor
- [`ft_putendl_fd`](#ft_putendl_fd) - Output string with newline to file descriptor
- [`ft_putnbr_fd`](#ft_putnbr_fd) - Output integer to file descriptor
- [`ft_printf`](#ft_printf) - Formatted output conversion

### Linked List Helpers (List/)
- [`ft_lstnew`](#ft_lstnew) - Create new list node
- [`ft_lstsize`](#ft_lstsize) - Count list nodes
- [`ft_lstlast`](#ft_lstlast) - Get last list node
- [`ft_lstadd_front`](#ft_lstadd_front) - Add node at list beginning
- [`ft_lstadd_back`](#ft_lstadd_back) - Add node at list end
- [`ft_lstdelone`](#ft_lstdelone) - Delete single node
- [`ft_lstclear`](#ft_lstclear) - Delete and free entire list
- [`ft_lstiter`](#ft_lstiter) - Apply function to each node
- [`ft_lstmap`](#ft_lstmap) - Apply function and create new list
- [`ft_lstprint_nbr`](#ft_lstprint_nbr) - Print list content as integers

---

## Character Classification & Case Conversion (Char/)

### `ft_isalnum`
```c
int ft_isalnum(int c);
```
**Parameters:**
- `c` - Character to test (as `unsigned char` cast to `int`, or `EOF`)

**Returns:** Non-zero if `c` is alphanumeric (`A-Z`, `a-z`, `0-9`), otherwise `0`.

**Description:** Tests whether the character is an ASCII letter or digit.

---

### `ft_isalpha`
```c
int ft_isalpha(int c);
```
**Parameters:**
- `c` - Character to test

**Returns:** Non-zero if `c` is an ASCII letter (`A-Z` or `a-z`), otherwise `0`.

**Description:** Tests whether the character is alphabetic.

---

### `ft_isascii`
```c
int ft_isascii(int c);
```
**Parameters:**
- `c` - Character to test

**Returns:** Non-zero if `c` is within the 7-bit ASCII range (`0-127`), otherwise `0`.

**Description:** Checks if the value falls within the standard ASCII character set.

---

### `ft_isdigit`
```c
int ft_isdigit(int c);
```
**Parameters:**
- `c` - Character to test

**Returns:** Non-zero if `c` is an ASCII digit (`'0'` through `'9'`), otherwise `0`.

**Description:** Tests whether the character is a decimal digit.

---

### `ft_isprint`
```c
int ft_isprint(int c);
```
**Parameters:**
- `c` - Character to test

**Returns:** Non-zero if `c` is a printable character (including space), otherwise `0`.

**Description:** Tests whether the character is printable, including the space character (0x20).

---

### `ft_tolower`
```c
int ft_tolower(int c);
```
**Parameters:**
- `c` - Character to convert

**Returns:** The lowercase equivalent of `c` if it's an uppercase letter, otherwise `c` unchanged.

**Description:** Converts uppercase ASCII letters to lowercase. Non-letter characters are returned unchanged.

---

### `ft_toupper`
```c
int ft_toupper(int c);
```
**Parameters:**
- `c` - Character to convert

**Returns:** The uppercase equivalent of `c` if it's a lowercase letter, otherwise `c` unchanged.

**Description:** Converts lowercase ASCII letters to uppercase. Non-letter characters are returned unchanged.

---

## String Utilities (Strings/)

### `ft_strlen`
```c
size_t ft_strlen(const char *s);
```
**Parameters:**
- `s` - Pointer to a null-terminated string

**Returns:** The number of characters in `s`, excluding the null terminator.

**Description:** Computes the length of the string by counting bytes until the null byte `\0`.

---

### `ft_strdup`
```c
char *ft_strdup(const char *s1);
```
**Parameters:**
- `s1` - Source string to duplicate

**Returns:** A pointer to the newly allocated string, or `NULL` if allocation fails.

**Description:** Allocates memory and copies the string `s1` (including the null terminator). The caller must free the returned pointer.

---

### `ft_strchr`
```c
char *ft_strchr(const char *s, int c);
```
**Parameters:**
- `s` - String to search
- `c` - Character to find (converted to `char`)

**Returns:** Pointer to the first occurrence of `c` in `s`, or `NULL` if not found.

**Description:** Locates the first occurrence of `c` in the string `s`. The null terminator is considered part of the string.

---

### `ft_strrchr`
```c
char *ft_strrchr(const char *s, int c);
```
**Parameters:**
- `s` - String to search
- `c` - Character to find

**Returns:** Pointer to the last occurrence of `c` in `s`, or `NULL` if not found.

**Description:** Locates the last occurrence of `c` in the string `s`.

---

### `ft_strncmp`
```c
int ft_strncmp(const char *s1, const char *s2, size_t n);
```
**Parameters:**
- `s1` - First string to compare
- `s2` - Second string to compare
- `n` - Maximum number of bytes to compare

**Returns:** 
- `< 0` if `s1` is less than `s2`
- `0` if `s1` equals `s2`
- `> 0` if `s1` is greater than `s2`

**Description:** Compares up to `n` bytes of `s1` and `s2` lexicographically.

---

### `ft_strnstr`
```c
char *ft_strnstr(const char *haystack, const char *needle, size_t len);
```
**Parameters:**
- `haystack` - String to search in
- `needle` - String to search for
- `len` - Maximum number of characters to search

**Returns:** Pointer to the first occurrence of `needle` in `haystack`, or `NULL` if not found within `len` bytes.

**Description:** Locates the first occurrence of the null-terminated string `needle` in `haystack`, searching at most `len` characters.

---

### `ft_strlcpy`
```c
size_t ft_strlcpy(char *dst, const char *src, size_t dstsize);
```
**Parameters:**
- `dst` - Destination buffer
- `src` - Source string
- `dstsize` - Size of destination buffer

**Returns:** The length of `src`.

**Description:** Copies up to `dstsize - 1` characters from `src` to `dst`, always null-terminating the result if `dstsize > 0`. Returns the total length of `src` to allow detection of truncation.

---

### `ft_strlcat`
```c
size_t ft_strlcat(char *dst, const char *src, size_t dstsize);
```
**Parameters:**
- `dst` - Destination buffer (must contain a null-terminated string)
- `src` - Source string to append
- `dstsize` - Full size of destination buffer

**Returns:** The total length of the string it tried to create (initial length of `dst` + length of `src`).

**Description:** Appends `src` to `dst`, ensuring the result is null-terminated within `dstsize`. Returns the length that would have been created to detect truncation.

---

### `ft_substr`
```c
char *ft_substr(char const *s, unsigned int start, size_t len);
```
**Parameters:**
- `s` - Source string
- `start` - Starting index in `s`
- `len` - Maximum length of substring

**Returns:** A newly allocated substring, or `NULL` on allocation failure.

**Description:** Allocates and returns a substring from `s` starting at index `start` with maximum length `len`. If `start` exceeds the string length, returns an empty string.

---

### `ft_strjoin`
```c
char *ft_strjoin(char const *s1, char const *s2);
```
**Parameters:**
- `s1` - First string
- `s2` - Second string

**Returns:** A newly allocated string containing the concatenation of `s1` and `s2`, or `NULL` on allocation failure.

**Description:** Allocates and returns a new string resulting from the concatenation of `s1` followed by `s2`.

---

### `ft_strtrim`
```c
char *ft_strtrim(char const *s1, char const *set);
```
**Parameters:**
- `s1` - String to trim
- `set` - Set of characters to remove

**Returns:** A newly allocated trimmed string, or `NULL` on allocation failure.

**Description:** Allocates and returns a copy of `s1` with all characters in `set` removed from the beginning and end.

---

### `ft_split`
```c
char **ft_split(char const *s, char c);
```
**Parameters:**
- `s` - String to split
- `c` - Delimiter character

**Returns:** A `NULL`-terminated array of newly allocated strings, or `NULL` on allocation failure.

**Description:** Splits `s` using `c` as delimiter and returns an array of strings. Each substring is allocated separately. The array itself and all strings must be freed by the caller.

---

### `ft_strmapi`
```c
char *ft_strmapi(char const *s, char (*f)(unsigned int, char));
```
**Parameters:**
- `s` - Source string
- `f` - Function to apply to each character (receives index and character)

**Returns:** A newly allocated string with `f` applied to each character, or `NULL` on allocation failure.

**Description:** Creates a new string by applying function `f` to each character of `s`, passing the character's index as the first argument.

---

### `ft_striteri`
```c
void ft_striteri(char *s, void (*f)(unsigned int, char *));
```
**Parameters:**
- `s` - String to iterate over
- `f` - Function to apply to each character (receives index and pointer to character)

**Returns:** Nothing.

**Description:** Applies function `f` to each character of `s`, passing the character's index and a pointer to the character itself, allowing in-place modification.

---

## Memory Management (Memory/)

### `ft_memset`
```c
void *ft_memset(void *b, int c, size_t len);
```
**Parameters:**
- `b` - Pointer to memory block
- `c` - Value to set (converted to `unsigned char`)
- `len` - Number of bytes to set

**Returns:** The original pointer `b`.

**Description:** Fills the first `len` bytes of the memory area pointed to by `b` with the constant byte `c`.

---

### `ft_bzero`
```c
void ft_bzero(void *s, size_t n);
```
**Parameters:**
- `s` - Pointer to memory block
- `n` - Number of bytes to zero

**Returns:** Nothing.

**Description:** Sets the first `n` bytes of the memory area pointed to by `s` to zero.

---

### `ft_memcpy`
```c
void *ft_memcpy(void *dest, const void *src, size_t n);
```
**Parameters:**
- `dest` - Destination memory area
- `src` - Source memory area
- `n` - Number of bytes to copy

**Returns:** The original pointer `dest`.

**Description:** Copies `n` bytes from memory area `src` to `dest`. The memory areas must not overlap; use `ft_memmove` for overlapping regions.

---

### `ft_memmove`
```c
void *ft_memmove(void *dst, const void *src, size_t len);
```
**Parameters:**
- `dst` - Destination memory area
- `src` - Source memory area
- `len` - Number of bytes to copy

**Returns:** The original pointer `dst`.

**Description:** Copies `len` bytes from `src` to `dst`. The memory areas may overlap; copying is done in a safe manner to handle this correctly.

---

### `ft_memchr`
```c
void *ft_memchr(const void *s, int c, size_t n);
```
**Parameters:**
- `s` - Memory area to search
- `c` - Byte value to find (converted to `unsigned char`)
- `n` - Number of bytes to examine

**Returns:** Pointer to the first occurrence of `c` in the first `n` bytes of `s`, or `NULL` if not found.

**Description:** Scans the first `n` bytes of the memory area `s` for the first instance of `c`.

---

### `ft_memcmp`
```c
int ft_memcmp(const void *s1, const void *s2, size_t n);
```
**Parameters:**
- `s1` - First memory area
- `s2` - Second memory area
- `n` - Number of bytes to compare

**Returns:**
- `< 0` if `s1` is less than `s2`
- `0` if equal
- `> 0` if `s1` is greater than `s2`

**Description:** Compares the first `n` bytes of memory areas `s1` and `s2`.

---

### `ft_calloc`
```c
void *ft_calloc(size_t count, size_t size);
```
**Parameters:**
- `count` - Number of elements
- `size` - Size of each element

**Returns:** A pointer to the allocated and zeroed memory, or `NULL` on failure (including overflow).

**Description:** Allocates memory for an array of `count` elements of `size` bytes each and zeroes the memory. Guards against integer overflow in the multiplication.

---

### `ft_free_array`
```c
void ft_free_array(char **arr);
```
**Parameters:**
- `arr` - NULL-terminated array of strings to free

**Returns:** Nothing.

**Description:** Frees each string in the array and then the array pointer itself.

---

## Conversion Utilities (Conversion/)

### `ft_atoi`
```c
int ft_atoi(const char *nptr);
```
**Parameters:**
- `nptr` - String to parse

**Returns:** The converted integer value.

**Description:** Parses the initial portion of `nptr` as a signed decimal integer. Skips leading whitespace, handles an optional `+` or `-` sign, and converts digits until a non-digit character is encountered.

---

### `ft_atol`
```c
long ft_atol(const char *nptr);
```
**Parameters:**
- `nptr` - String to parse

**Returns:** The converted long integer value.

**Description:** Parses the initial portion of `nptr` as a signed decimal long integer. Skips leading whitespace, handles an optional `+` or `-` sign, and converts digits until a non-digit character is encountered. Similar to `ft_atoi` but returns a `long` for larger number ranges.

---

### `ft_itoa`
```c
char *ft_itoa(int n);
```
**Parameters:**
- `n` - Integer to convert

**Returns:** A newly allocated string representing `n`, or `NULL` on allocation failure.

**Description:** Allocates and returns a string representing the integer `n`. Handles negative numbers correctly, including `INT_MIN`.

---

### `ft_overint`
```c
int ft_overint(long n);
```
**Parameters:**
- `n` - Long integer to check

**Returns:** `1` if `n` is outside the range of `int` (`INT_MIN` to `INT_MAX`), `0` otherwise.

**Description:** Checks if a long integer value overflows or underflows the standard `int` range.

---

## File Descriptor Output Helpers (Output/)

### `ft_putchar_fd`
```c
void ft_putchar_fd(char c, int fd);
```
**Parameters:**
- `c` - Character to write
- `fd` - File descriptor to write to

**Returns:** Nothing.

**Description:** Writes the character `c` to the specified file descriptor.

---

### `ft_putstr_fd`
```c
void ft_putstr_fd(char *s, int fd);
```
**Parameters:**
- `s` - String to write
- `fd` - File descriptor to write to

**Returns:** Nothing.

**Description:** Writes the string `s` to the specified file descriptor.

---

### `ft_putendl_fd`
```c
void ft_putendl_fd(char *s, int fd);
```
**Parameters:**
- `s` - String to write
- `fd` - File descriptor to write to

**Returns:** Nothing.

**Description:** Writes the string `s` followed by a newline character to the specified file descriptor.

---

### `ft_putnbr_fd`
```c
void ft_putnbr_fd(int n, int fd);
```
**Parameters:**
- `n` - Integer to write
- `fd` - File descriptor to write to

**Returns:** Nothing.

**Description:** Writes the integer `n` as decimal digits to the specified file descriptor. Handles negative numbers correctly.

---

### `ft_printf`
```c
int ft_printf(const char *s, ...);
```
**Parameters:**
- `s` - Format string
- `...` - Variable arguments

**Returns:** The number of characters printed.

**Description:** A simplified implementation of the standard `printf` function. Supports various format specifiers.

---

## Linked List Helpers (List/)

### List Structure
```c
typedef struct s_list
{
    void            *content;
    struct s_list   *next;
} t_list;
```

---

### `ft_lstnew`
```c
t_list *ft_lstnew(void *content);
```
**Parameters:**
- `content` - Pointer to the content for the new node

**Returns:** A pointer to the new node, or `NULL` on allocation failure.

**Description:** Allocates and returns a new list node with `content` set to the parameter and `next` set to `NULL`.

---

### `ft_lstsize`
```c
int ft_lstsize(t_list *lst);
```
**Parameters:**
- `lst` - Head of the list

**Returns:** The number of nodes in the list.

**Description:** Counts and returns the number of nodes in the list.

---

### `ft_lstlast`
```c
t_list *ft_lstlast(t_list *lst);
```
**Parameters:**
- `lst` - Head of the list

**Returns:** A pointer to the last node, or `NULL` if the list is empty.

**Description:** Returns the last node of the list.

---

### `ft_lstadd_front`
```c
void ft_lstadd_front(t_list **lst, t_list *new);
```
**Parameters:**
- `lst` - Address of a pointer to the first node of the list
- `new` - Pointer to the node to add

**Returns:** Nothing.

**Description:** Adds the node `new` at the beginning of the list.

---

### `ft_lstadd_back`
```c
void ft_lstadd_back(t_list **lst, t_list *new);
```
**Parameters:**
- `lst` - Address of a pointer to the first node of the list
- `new` - Pointer to the node to add

**Returns:** Nothing.

**Description:** Adds the node `new` at the end of the list.

---

### `ft_lstdelone`
```c
void ft_lstdelone(t_list *lst, void (*del)(void *));
```
**Parameters:**
- `lst` - Node to free
- `del` - Function to delete the node's content

**Returns:** Nothing.

**Description:** Frees the memory of the node's content using the function `del`, then frees the node itself. The `next` pointer is not freed.

---

### `ft_lstclear`
```c
void ft_lstclear(t_list **lst, void (*del)(void *));
```
**Parameters:**
- `lst` - Address of a pointer to the first node
- `del` - Function to delete each node's content

**Returns:** Nothing.

**Description:** Deletes and frees all nodes in the list using `del` and `free`. Sets the list pointer to `NULL`.

---

### `ft_lstiter`
```c
void ft_lstiter(t_list *lst, void (*f)(void *));
```
**Parameters:**
- `lst` - Head of the list
- `f` - Function to apply to each node's content

**Returns:** Nothing.

**Description:** Iterates over the list and applies function `f` to the content of each node.

---

### `ft_lstmap`
```c
t_list *ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));
```
**Parameters:**
- `lst` - Head of the list to map
- `f` - Function to apply to each node's content
- `del` - Function to delete content if allocation fails

**Returns:** A pointer to the new list, or `NULL` on allocation failure.

**Description:** Creates a new list by applying function `f` to each node's content in `lst`. If any allocation fails, uses `del` to free all allocated nodes and returns `NULL`.

---

### `ft_lstprint_nbr`
```c
void ft_lstprint_nbr(t_list *lst);
```
**Parameters:**
- `lst` - Head of the list to print

**Returns:** Nothing.

**Description:** Iterates through the list and prints the content of each node as an integer followed by a newline. Assumes `lst->content` holds an integer value.

---

## Building & Usage

Compile the library using the provided `Makefile`:

```sh
make        # build mandatory functions into libft.a
make bonus  # include bonus linked-list helpers
make clean  # remove object files
make fclean # remove object files and libft.a
make re     # rebuild from scratch
```

Link `libft.a` in your projects:

```sh
cc -Wall -Wextra -Werror main.c libft.a -I. -o my_program
```

Include the header in your source files:

```c
#include "libft.h"
```
