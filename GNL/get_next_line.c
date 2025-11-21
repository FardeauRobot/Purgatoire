/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:24:16 by tibras            #+#    #+#             */
/*   Updated: 2025/11/21 17:14:27 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
/*
NOTES : 



// 
CHAR * GNNL(FD ENTRÉE)
{
	char r_buffer[BUFFER_SIZE];
	char *rendu;
	statc t_list *l_buffer[4096];
	ssize_t		size_read;

	Si fd < 0 || Impossible de lire fd)
		return (NULL);
	size_read = read(fd, r_buffer, BUFFER_SIZE);	
	Tant que le nbr de caracteres lus > 0 :
	{ 
		Si on trouve un \n dans buffer		
			On copie l_buffer dans rendu;
			Copie la partie buffer dans rendu:
			On clean l_buffer;
			l_buffer = r_buffer[a partir de \n];
			return (rendu);
		Sinon :
			on crée un nouveau noeud
			on les stocke dans l_buffer;
		size_read = read(fd, r_buffer, BUFFER_SIZE);	
	}
}
*/

// char	*ft_theline(char *arr, int fd)
// {
// 	ssize_t	n_read;
// 	char	*buf;
// 	char	*tmp;
// 	char	*delete;

// 	while (ft_strchr(arr, '\n') < 0)
// 	{
// 		buf = malloc(sizeof(char) * BUFFER_SIZE + 1);
// 		if (!buf)
// 			return (NULL);
// 		n_read = read(fd, buf, BUFFER_SIZE);
// 		// FIXED: Separated n_read < 0 from n_read == 0
// 		// Original had (n_read <= 0) which returned arr on error
// 		// Now: error returns NULL, EOF returns leftover arr
// 		if (n_read <= 0)
// 		{
// 			free(buf);
// 			if (n_read == 0)
// 				return (arr);
// 			return (NULL);
// 		}
// 		buf[n_read] = '\0';
// 		delete = arr;
// 		tmp = ft_strjoin(arr, buf);
// 		free(buf);
// 		free(delete);
// 		// FIXED: Added check for ft_strjoin failure
// 		// Original didn't check if tmp was NULL before assigning
// 		if (!tmp)
// 			return (NULL);
// 		arr = tmp;
// 	}
// 	return (arr);
// }

// char	*ft_found_newline(char **arr)
// {
// 	char *res;
// 	char *tmp;
// 	int	index;

// 	index = ft_strchr(*arr, '\n');
// 	// FIXED: Changed from 'index' to 'index + 1' to include the \n
// 	// Original: res = ft_mini_substr(*arr, index); (didn't include \n)
// 	res = ft_mini_substr(*arr, index + 1);
// 	if (!res)
// 	{
// 		free(*arr);
// 		*arr = NULL;
// 		return (NULL);
// 	}
// 	tmp = ft_mini_substr(*arr + index + 1, ft_strlen(*arr + index + 1));
// 	free(*arr);
// 	*arr = tmp;
// 	return (res);
// }

// char *get_next_line(int fd)
// {
// 	static char *arr_s[4096];
// 	char	buf[1];
// 	char *tmp;

// 	// FIXED: Added cleanup of static buffer on error
// 	// Original just returned NULL, leaking any previous allocation
// 	// This prevents "still reachable" memory in valgrind
// 	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, buf, 0) < 0)
// 	{
// 		if (arr_s[fd])
// 		{
// 			free(arr_s[fd]);
// 			arr_s[fd] = NULL;
// 		}
// 		return (NULL);
// 	}
// 	arr_s[fd] = ft_theline(arr_s[fd], fd);
// 	if (!arr_s[fd])
// 		return (NULL);
// 	// FIXED: Added check for empty string at EOF
// 	// Original didn't handle EOF when buffer is empty
// 	if (!arr_s[fd][0])
// 	{
// 		free(arr_s[fd]);
// 		arr_s[fd] = NULL;
// 		return (NULL);
// 	}
// 	// FIXED: Check if newline exists before calling ft_found_newline
// 	// Original always called ft_found_newline, causing errors when
// 	// the last line has no \n (ft_strchr returns -1)
// 	if (ft_strchr(arr_s[fd], '\n') >= 0)
// 		tmp = ft_found_newline(&arr_s[fd]);
// 	else
// 	{
// 		// No newline found: return entire buffer (last line)
// 		tmp = arr_s[fd];
// 		arr_s[fd] = NULL;
// 	}
// 	return (tmp);
// }

char	*ft_find_line(char **arr, int fd)
{
	char 	*buf;
	char	*tmp;
	int		n_read;

	// Tant que pas de \n dans arr
	while (!ft_strchr(*arr, '\0'))
	{
		// Creer le buffer
		buf = malloc(BUFFER_SIZE + 1);
		if (!buf)
			return (NULL);
		buf[BUFFER_SIZE] = '\0';
		// Remplir buffer
		n_read = read(fd, buf, BUFFER_SIZE)	;
		if (n_read < 0)
			return (NULL);
		// Additionner arr et buf
		tmp = ft_strjoin(*arr, buf);
		// On libere les 2 parties
		free (*arr);
		free (buf);
		// On associe tmp a arr
		*arr = tmp;
	}
	printf("ARR = %s\n", *arr);
	return (*arr);
}

char	*ft_create_res(char *s)
{
	char	*tmp;
	int		max;
	int		i;

	max = ft_strchr((const char *)s, '\n');
	tmp = 
	while ()
	return (tmp);
}

char	*get_next_line(int fd)
{
	static char *arr_s[4096];
	char	*res;
	char	check[1];

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, check, 0))
		return (NULL);
	// Trouve
	res = ft_find_line(&arr_s[fd], fd);
	res = ft_create_res(res);
	arr_s[fd] = ft_cleanup(res);
	return (res);
}

int	main()
{
	int fd1;
	int i = 0;
	// int fd2;
	char *res;

	fd1 = open("test1", O_RDONLY);
	// fd2 = open("test2", O_RDONLY);
	res= get_next_line(fd1);
	printf("%s\n", res);
	// while (i < 5)
	// {
	// 	printf("%s\n", res);
	// 	free(res);
	// 	res= get_next_line(fd1);
	// 	i++;
	// }
	free(res);
	close (fd1);
	return (0);
}