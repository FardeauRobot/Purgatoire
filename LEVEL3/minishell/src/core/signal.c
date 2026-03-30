/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 14:09:27 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/21 19:07:35 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	signal_callback_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_glob = sig;
		ft_printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	setup_signal(void)
{
	struct sigaction	sa;

	g_signal_glob = 0;
	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = &signal_callback_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror(ERR_SIG_INT);
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		perror(ERR_SIG_IGN_QUIT);
}
