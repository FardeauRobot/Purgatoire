/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 21:50:28 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/25 21:58:07 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SORTING_H
# define SORTING_H

# include <stddef.h>

void	ft_bubble_sort(int *arr, size_t size);
void	ft_selection_sort(int *arr, size_t size);
void	ft_insertion_sort(int *arr, size_t size);
void	ft_merge_sort(int *arr, size_t size);
void	ft_qsort(int *arr, size_t size);
void	ft_heap_sort(int *arr, size_t size);
void	ft_shell_sort(int *arr, size_t size);
void	ft_counting_sort(int *arr, size_t size, int min_val, int max_val);
void	ft_radix_sort(int *arr, size_t size, int base);
void	ft_bucket_sort(int *arr, size_t size, size_t bucket_count);

#endif