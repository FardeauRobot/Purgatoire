/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 17:25:06 by tibras            #+#    #+#             */
/*   Updated: 2026/01/21 18:01:40 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

void	ft_free_arr_pipe(t_pipe **arr_pipe, int size)
{
	int i;

	i = 0;
	while (i < size)
	{
		if (arr_pipe[i]->cmd)
			free(arr_pipe[i]->cmd);
		if (arr_pipe[i]->path)
			free(arr_pipe[i]->path);
		if (arr_pipe[i]->arg_pipe)
			ft_free_array(arr_pipe[i]->arg_pipe);
		free(arr_pipe[i]);
		i++;
	}
	free(arr_pipe);
}