/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:04:15 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/21 15:09:54 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_chishell_logo(void)
{
	printf("\n\n\033[1;36m");
	printf(" ██████ ██   ██ ██ ███████ ");
	printf("██   ██ ███████ ██      ██      \n");
	printf("██      ██   ██ ██ ██      ");
	printf("██   ██ ██      ██      ██      \n");
	printf("██      ███████ ██ ███████ ");
	printf("███████ █████   ██      ██      \n");
	printf("██      ██   ██ ██      ██ ");
	printf("██   ██ ██      ██      ██      \n");
	printf(" ██████ ██   ██ ██ ███████ ");
	printf("██   ██ ███████ ███████ ███████ \n");
	printf("\033[0m");
}

void	print_header(void)
{
	print_chishell_logo();
	printf("\n\033[1;35m");
	printf("          /\\_ /\\ \n");
	printf("         (  - .-)  \033[3m~ chillin'...\033[0;1;35m\n");
	printf("          >  ^  < \n");
	printf("\033[1;37m");
	printf("%63s\n", "by alamjada and tibras");
	printf("\033[0m\n");
}
