/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:01:23 by tibras            #+#    #+#             */
/*   Updated: 2025/11/29 18:27:50 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_arr_clean(char **arr, char *tmp, int mod)
{
	free(*arr);
	if (mod == 0)
		*arr = NULL;
	else if (mod)
		*arr = tmp;
	return (*arr);
}

static char	*ft_read_check(ssize_t n_read, char *arr, char **buf)
{
	free(*buf);
	if (n_read == 0)
		return (arr);
	free(arr);
	return (NULL);
}

static char	*ft_theline(char *arr, int fd)
{
	ssize_t	n_read;
	char	*buf;
	char	*tmp;
	char	*delete;

	n_read = 1;
	while (ft_gnl_strchr(arr, '\n') < 0)
	{
		buf = malloc(sizeof(char) * BUFFER_SIZE + 1);
		if (!buf)
			return (ft_read_check(n_read, arr, &buf));
		n_read = read(fd, buf, BUFFER_SIZE);
		if (n_read <= 0)
			return (ft_read_check(n_read, arr, &buf));
		buf[n_read] = '\0';
		delete = arr;
		tmp = ft_gnl_strjoin(arr, buf);
		ft_read_check(1, delete, &buf);
		if (!tmp)
			return (NULL);
		arr = tmp;
	}
	return (arr);
}

static char	*ft_found_newline(char **arr)
{
	char	*res;
	char	*tmp;
	int		index;

	index = ft_gnl_strchr(*arr, '\n');
	res = ft_gnl_substr(*arr, index + 1);
	if (!res)
		return (ft_arr_clean(arr, NULL, 0));
	tmp = ft_gnl_substr(*arr + index + 1, ft_gnl_strlen(*arr + index + 1));
	if (!tmp)
	{
		free(res);
		return (ft_arr_clean(arr, NULL, 0));
	}
	ft_arr_clean(arr, tmp, 1);
	return (res);
}

char	*get_next_line(int fd)
{
	static char		*arr_s[1024];
	char			buf[1];
	char			*tmp;

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, buf, 0) < 0)
	{
		if (arr_s[fd])
			ft_arr_clean(&arr_s[fd], NULL, 0);
		return (NULL);
	}
	arr_s[fd] = ft_theline(arr_s[fd], fd);
	if (!arr_s[fd])
		return (NULL);
	if (!arr_s[fd][0])
		return (ft_arr_clean(&arr_s[fd], NULL, 0));
	if (ft_gnl_strchr(arr_s[fd], '\n') >= 0)
		tmp = ft_found_newline(&arr_s[fd]);
	else
	{
		tmp = arr_s[fd];
		arr_s[fd] = NULL;
	}
	return (tmp);
}
