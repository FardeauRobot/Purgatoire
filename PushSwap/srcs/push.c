/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 11:59:32 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 13:14:17 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void	ft_push(t_list **stack_src, t_list **stack_dest)
{
	t_list	*tmp;

	if (!*stack_src)
		return ;
	tmp = *stack_src;
	*stack_src = stack_src->next;
	tmp->next = *stack_dest;
	tmp->content->target = NULL;
	*stack_dest = tmp;
}

void	ft_pa(t_list **stack_b, t_list **stack_a, int check)
{
	ft_push(stack_b, stack_a);
	if (check)
		ft_printf("pa\n");
}

void	ft_pb(t_list **stack_a, t_list **stack_b, int check)
{
	ft_push(stack_a, stack_b);
	if (check)
		ft_printf("pb\n");
}