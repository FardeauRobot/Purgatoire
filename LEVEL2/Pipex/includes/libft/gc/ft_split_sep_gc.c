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

static char	**ft_exec_gc(const char *s, char sep, \
	size_t w_count, t_list **gc_head)
{
	char	**arr;
	size_t	index;
	size_t	i[2];

	arr = ft_calloc_gc(w_count + 1, sizeof(char *), gc_head);
	if (!arr)
		return (NULL);
	i[0] = 0;
	index = 0;
	while (index < w_count)
	{
		while (s[i[0]] && s[i[0]] == sep)
			i[0]++;
		i[1] = i[0];
		while (s[i[0]] && s[i[0]] != sep)
			i[0]++;
		arr[index] = ft_cust_strdup_gc(s, i[1], i[0], gc_head);
		if (!arr[index])
			return (NULL);
		index++;
	}
	arr[index] = NULL;
	return (arr);
}

char	**ft_split_sep_gc(char const *s, char sep, t_list **gc_head)
{
	size_t	word_count;

	word_count = ft_count_word(s, sep);
	return (ft_exec_gc(s, sep, word_count, gc_head));
}
