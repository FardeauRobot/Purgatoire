/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 14:41:58 by tibras            #+#    #+#             */
/*   Updated: 2025/12/03 16:23:19 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

static	void	free_tab(char **arr, size_t index)
{
	size_t	i;

	i = 0;
	while (i <= index)
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

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

static char	*ft_cust_strdup(const char *s, size_t start, size_t end)
{
	char	*tmp;
	size_t	range;
	size_t	i;

	range = end - start;
	tmp = malloc(sizeof(char) * range + 1);
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

static char	**ft_exec(char **arr, const char *s, char sep, size_t w_count)
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
		arr[index] = ft_cust_strdup(s, start, i);
		if (!arr[index])
		{
			free_tab(arr, index);
			return (NULL);
		}
		index++;
	}
	arr[index] = NULL;
	return (arr);
}

char	**ft_split_sep(char const *s, char sep)
{
	char	**tab_arr;
	size_t	word_count;

	if (!s)
		return (NULL);
	word_count = ft_count_word(s, sep);
	tab_arr = malloc(sizeof(char *) * (word_count + 1));
	if (!tab_arr)
		return (NULL);
	tab_arr = ft_exec(tab_arr, s, sep, word_count);
	if (!tab_arr)
		return (NULL);
	return (tab_arr);
}

// int main ()
// {
// 	char const *test = "--1-2--3---4----5-----42";
// 	char sep = '-';
// 	char **test_arr;
// 	int i = 0;

// 	test_arr = ft_split(test, sep);
// 	while (i < 7)
// 	{
// 		printf("ARR[%d] = %s\n", i, test_arr[i]);
// 		free(test_arr[i]);
// 		i++;
// 	}
// 	free(test_arr);
// 	ft_split(test, sep);
// }