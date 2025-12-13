/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:09:53 by tibras            #+#    #+#             */
/*   Updated: 2025/12/13 18:40:30 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	main(int argc, char **argv)
{
	t_list	*stack_a;
	t_list	*stack_b;

	if (argc < 2)
		return (1);
	stack_a = NULL;
	stack_b = NULL;
	ft_parsing(&stack_a, argc, argv);
	if (ft_no_double(stack_a))
		exit(ft_error_parse(NULL, &stack_a));
	if (ft_is_sorted(&stack_a, &stack_b))
	{
		ft_lstclear(&stack_a, free);
		return (0);
	}
	ft_sorting(&stack_a, &stack_b);
	ft_lstprint_both(stack_a, stack_b);
	return (0);
}
