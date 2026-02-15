/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_sep_gc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 09:50:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_count_word(char const *s, char sep)
{
	size_t	count;
	size_t	i;

	if (!s)
		return (0);
	else if (!sep && s[0])
		return (1);
	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != sep && (i == 0 || s[i - 1] == sep))
			count++;
		i++;
	}
	return (count);
}

static char	*ft_cust_strdup_gc(const char *s, size_t start, size_t end, \
	t_list **gc_head)
{
	char	*tmp;
	size_t	range;
	size_t	i;

	range = end - start;
	tmp = ft_calloc_gc(range + 1, sizeof(char), gc_head);
	if (!tmp)
		return (NULL);
	i = 0;
	while (start + i < end)
	{
		tmp[i] = s[start + i];
		i++;
	}
	tmp[i] = '\0';
	return (tmp);
}

static char	**ft_exec_gc(char **arr, const char *s, char sep, \
	size_t w_count, t_list **gc_head)
{
	size_t	index;
	size_t	start;
	size_t	i;

	start = 0;
	i = 0;
	index = 0;
	while (index < w_count)
	{
		while (s[i] && s[i] == sep)
			i++;
		start = i;
		while (s[i] && s[i] != sep)
			i++;
		arr[index] = ft_cust_strdup_gc(s, start, i, gc_head);
		if (!arr[index])
			return (NULL);
		index++;
	}
	arr[index] = NULL;
	return (arr);
}

char	**ft_split_sep_gc(char const *s, char sep, t_list **gc_head)
{
	char	**tab_arr;
	size_t	word_count;

	word_count = ft_count_word(s, sep);
	tab_arr = ft_calloc_gc(word_count + 1, sizeof(char *), gc_head);
	if (!tab_arr)
		return (NULL);
	if (!ft_exec_gc(tab_arr, s, sep, word_count, gc_head))
		return (NULL);
	return (tab_arr);
}
