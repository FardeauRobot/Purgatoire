/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:09:53 by tibras            #+#    #+#             */
/*   Updated: 2025/12/10 18:36:16 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	main(int argc, char **argv)
{
	t_list	*stack_a;
	t_list	*stack_b;
	int		error;

	error = 0;
	if (argc < 2)
		return (1);
	stack_a = NULL;
	stack_b = NULL;
	ft_parsing(&stack_a, argc, argv);
	if (!ft_is_sorted(stack_a) || ft_no_double(stack_a))
	{
		ft_full_free(NULL, &stack_a, free);
		return (ft_print_error(error));
	}
	ft_sorting(&stack_a, &stack_b);
	ft_lstclear(&stack_a, free);
	return (0);
}
