# Libft Functions

## Table of Contents
- [Char](#char)
- [Conversion](#conversion)
- [Garbage Collection (GC)](#garbage-collection-gc)
- [GNL](#gnl)
- [List](#list)
- [Memory](#memory)
- [Output](#output)
- [Strings](#strings)

## Char
- `ft_isalnum`: Check if character is alphanumeric.
- `ft_isalpha`: Check if character is alphabetic.
- `ft_isascii`: Check if character is in ASCII table.
- `ft_ischarset`: Check if character is present in a charset string.
- `ft_isdigit`: Check if character is a digit.
- `ft_isprint`: Check if character is printable.
- `ft_isspace`: Check if character is a whitespace.
- `ft_toupper`: Convert character to uppercase.
- `ft_tolower`: Convert character to lowercase.
- `ft_is_only`: Check if string consists only of a specific character.

## Conversion
- `ft_atoi`: Convert ASCII string to integer.
- `ft_atol`: Convert ASCII string to long.
- `ft_atof`: Convert ASCII string to float.
- `ft_itoa`: Convert integer to ASCII string.
- `ft_overint`: Check if string representation overflows integer limits.
- `ft_get_max`: Get the maximum value.
- `ft_min_int`: Get the minimum integer.
- `ft_max_int`: Get the maximum integer.

## Garbage Collection (GC)
- `ft_gc_malloc`: Allocate memory using garbage collection (wraps ft_calloc_gc).
- `ft_gc_add_node`: Add a pointer to the garbage collector list.
- `ft_gc_free_all`: Free all memory tracked by the garbage collector.
- `ft_calloc_gc`: Allocate and zero-initialize memory with garbage collection.
- `ft_itoa_gc`: GC-managed version of ft_itoa.
- `ft_lstnew_gc`: GC-managed version of ft_lstnew.
- `ft_lstmap_gc`: GC-managed version of ft_lstmap.
- `ft_strdup_gc`: GC-managed version of ft_strdup.
- `ft_substr_gc`: GC-managed version of ft_substr.
- `ft_strjoin_gc`: GC-managed version of ft_strjoin.
- `ft_strtrim_gc`: GC-managed version of ft_strtrim.
- `ft_strmapi_gc`: GC-managed version of ft_strmapi.
- `ft_strndup_gc`: GC-managed version of ft_strndup.
- `ft_split_charset_gc`: GC-managed version of ft_split_charset.
- `ft_split_sep_gc`: GC-managed version of ft_split_sep.

## GNL
- `get_next_line`: Read a line from a file descriptor.

## List
- `ft_lstadd_back`: Add element to end of list.
- `ft_lstadd_front`: Add element to front of list.
- `ft_lstclear`: Delete and free list.
- `ft_lstdelone`: Delete a node from list.
- `ft_lstiter`: Apply function to content of all list's elements.
- `ft_lstlast`: Get the last element of list.
- `ft_lstmap`: Apply function to content of all list's elements into new list.
- `ft_lstnew`: Create new list node.
- `ft_lstsize`: Count elements of a list.
- `ft_lstindex`: Get element at index.
- `ft_lstat_index`: Get index of element.

## Memory
- `ft_bzero`: Zero a byte string.
- `ft_calloc`: Allocate and zero-initialize memory.
- `ft_realloc`: Reallocate memory.
- `ft_memchr`: Locate byte in byte string.
- `ft_memcmp`: Compare byte string.
- `ft_memcpy`: Copy memory area.
- `ft_memmove`: Copy memory area.
- `ft_memset`: Fill memory with a constant byte.
- `ft_memdel`: Free and nullify memory.
- `ft_free_array`: Free an array of pointers.
- `ft_free_int_arr`: Free an array of integers.
- `ft_free_matrix`: Free a matrix.
- `ft_arr_swap`: Swap two arrays.
- `ft_memswap`: Swap memory areas.
- `ft_int_swap`: Swap two integers.
- `ft_int_cmp`: Compare two integers.

## Output
- `ft_putchar_fd`: Output a character to given file descriptor.
- `ft_putendl_fd`: Output a string to given file descriptor followed by newline.
- `ft_putnbr_fd`: Output integer to given file descriptor.
- `ft_putnbr_base`: Output integer in specific base.
- `ft_putstr_fd`: Output string to given file descriptor.
- `ft_printf`: Formatted output conversion.
- `ft_printf_nbr`: Helper for printf numbers.
- `ft_print_int_arr`: Print array of integers.

## Strings
- `ft_split_sep`: Split string by separator.
- `ft_split_charset`: Split string by charset.
- `ft_strchr`: Locate character in string.
- `ft_strcheck`: Check string content.
- `ft_strcmp`: Compare two strings.
- `ft_strdup`: Duplicate string.
- `ft_strndup`: Duplicate string up to n characters.
- `ft_striteri`: Apply function to string with index.
- `ft_strjoin`: Join two strings.
- `ft_strjoin_free`: Join two strings and free.
- `ft_strlcat`: Concatenate string to specific size.
- `ft_strlcpy`: Copy string to specific size.
- `ft_strlen`: Calculate length of string.
- `ft_strmapi`: Create new string by applying function to a string.
- `ft_strncmp`: Compare two strings up to n characters.
- `ft_strnstr`: Locate a substring in a string.
- `ft_strrchr`: Locate last occurrence of character in string.
- `ft_strtrim`: Trim characters from string.
- `ft_substr`: Extract substring from string.
- `ft_matrix_len`: Get length of matrix.
