/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:01:17 by tibras            #+#    #+#             */
/*   Updated: 2025/11/29 18:28:46 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_gnl_strlen(const char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

char	*ft_gnl_strjoin(const char *s1, const char *s2)
{
	char	*str;
	int		i;
	int		j;

	i = 0;
	j = 0;
	str = malloc(sizeof(char) * (ft_gnl_strlen(s1) + ft_gnl_strlen(s2)) + 1);
	if (!str)
		return (NULL);
	while (s1 && s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	while (s2 && s2[j])
	{
		str[i + j] = s2[j];
		j++;
	}
	str[i + j] = '\0';
	return (str);
}

int	ft_gnl_strchr(const char *s, int c)
{
	int				i;
	unsigned char	uc;

	if (!s || !s[0])
		return (-1);
	i = 0;
	uc = (unsigned char)c;
	while (s[i])
	{
		if (s[i] == uc)
			return (i);
		i++;
	}
	return (-1);
}

char	*ft_gnl_substr(const char *s, int max)
{
	int		i;
	char	*tmp;

	i = 0;
	tmp = malloc(sizeof(char) * max + 1);
	if (!tmp)
		return (NULL);
	while (i < max)
	{
		tmp[i] = s[i];
		i++;
	}
	tmp[i] = '\0';
	return (tmp);
}
