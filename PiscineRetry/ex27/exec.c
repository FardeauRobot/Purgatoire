/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:51:50 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:25:01 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

void	ft_display_file(char *filepath)
{
	int		fd;
	size_t	size;
	char	buf[1024];

	fd = open(filepath, O_RDONLY);
	size = 0;
	if (fd < 0)
	{
		ft_putstr_fd("Cannot read file.\n", 2);
		return ;
	}
	size = read(fd, buf, sizeof(buf));
	while (size > 0)
	{
		ft_putstrl(buf, size);
		size = read(fd, buf, sizeof(buf));
	}
	close(fd);
}
