/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_charset_gc.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 09:50:18 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_count_word(char const *s, char *charset)
{
	size_t	count;
	size_t	i;

	if (!s || !*s)
		return (0);
	i = 0;
	count = 0;
	while (s[i])
	{
		if (!ft_ischarset(s[i], charset)
			&& (i == 0 || ft_ischarset(s[i - 1], charset)))
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

static char	**ft_exec_gc(char **arr, const char *s, char *charset, \
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
		while (s[i] && ft_ischarset(s[i], charset))
			i++;
		start = i;
		while (s[i] && !ft_ischarset(s[i], charset))
			i++;
		arr[index] = ft_cust_strdup_gc(s, start, i, gc_head);
		if (!arr[index])
			return (NULL);
		index++;
	}
	arr[index] = NULL;
	return (arr);
}

char	**ft_split_charset_gc(char const *s, char *charset, t_list **gc_head)
{
	char	**tab_arr;
	size_t	word_count;

	if (!s)
		return (NULL);
	word_count = ft_count_word(s, charset);
	tab_arr = ft_calloc_gc(word_count + 1, sizeof(char *), gc_head);
	if (!tab_arr)
		return (NULL);
	if (!ft_exec_gc(tab_arr, s, charset, word_count, gc_head))
		return (NULL);
	return (tab_arr);
}
