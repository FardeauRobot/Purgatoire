/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memory.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 12:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/25 22:28:17 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MEMORY_H
# define FT_MEMORY_H

# include <stdlib.h>

// Memory manipulation functions
void	*ft_memset(void *b, int c, size_t len);
void	ft_bzero(void *s, size_t n);
void	*ft_memcpy(void *dest, const void *src, size_t n);
void	*ft_memmove(void *dst, const void *src, size_t len);
void	*ft_memchr(const void *s, int c, size_t n);
int		ft_memcmp(const void *s1, const void *s2, size_t n);

// Memory allocation functions
void	*ft_calloc(size_t count, size_t size);
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size);

// Memory freeing functions
void	ft_free_array(char **arr);
void	ft_free_int_arr(int **arr, int max);
void	ft_memdel(void **ap);
void	ft_free_matrix(char ***matrix);

// Array and integer manipulation functions
void	ft_arr_swap(int *arr, int len);
void	ft_memswap(void *a, void *b, size_t size);
void	ft_int_swap(int *a, int *b);
int		ft_int_cmp(int a, int b);

#endif
