/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/02 08:19:05 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "project.h"

/*
** safe_malloc - wrapper around malloc that exits on failure
** Avoids scattered NULL checks throughout the codebase.
*/
void	*safe_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		print_error("malloc", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return (ptr);
}

/*
** free_split - frees a NULL-terminated array of strings
** Typical for ft_split results and similar.
*/
void	free_split(char **tab)
{
	int	i;

	if (!tab)
		return ;
	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

/*
** free_data - frees all allocated fields in the t_data struct
** Extend this as you add heap-allocated fields to t_data.
*/
void	free_data(t_data *data)
{
	if (!data)
		return ;
	(void)data;
}
