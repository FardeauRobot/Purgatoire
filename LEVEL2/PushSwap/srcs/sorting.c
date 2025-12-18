/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:47:40 by tibras            #+#    #+#             */
/*   Updated: 2025/12/18 13:55:35 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

t_list	*ft_find_cheapest(t_list *stack)
{
	t_node	*n_cheapest;
	t_node	*n_current;
	t_list	*l_cheapest;

	l_cheapest = stack;
	while (stack)
	{
		n_cheapest = l_cheapest->content;
		n_current = stack->content;
		if (n_cheapest->cost > n_current->cost)
			l_cheapest = stack;
		stack = stack->next;
	}
	return (l_cheapest);
}

void	ft_get_cost(t_list **stack)
{
	int		l_len;
	int		index;
	t_list	*l_current;
	t_node	*n_current;

	l_current = *stack;
	l_len = ft_lstsize(*stack);
	while (l_current)
	{
		n_current = l_current->content;
		index = l_current->index;
		if (l_current->index > l_len / 2)
		{
			n_current->above = 0;
			n_current->to_top = l_len - index;
		}
		else
		{
			n_current->above = 1;
			n_current->to_top = index;
		}
		l_current = l_current->next;
	}
}

void	ft_compute_cost(t_list **stack_b)
{
	t_list	*l_current;
	t_list	*l_target;
	t_node	*n_target;
	t_node	*n_current;

	l_current = *stack_b;
	while (l_current)
	{
		n_current = ft_get_node(l_current);
		l_target = ft_get_target(n_current);
		n_target = ft_get_content(l_target);
		n_current->cost = n_current->to_top;
		n_current->cost += n_target->to_top;
		l_current = l_current->next;
	}
}

void	ft_affect_cost(t_list **stack_a, t_list **stack_b)
{
	ft_get_cost(stack_a);
	ft_get_cost(stack_b);
	ft_compute_cost(stack_b);
}

//  cherche la target dans la stack_a pour t_node
t_list	*ft_find_smallest(t_list *stack_a)
{
	t_node	*n_small;
	t_node	*node;
	t_list	*l_small;

	l_small = stack_a;
	n_small = ft_get_content(stack_a);
	while (stack_a)
	{
		node = ft_get_content(stack_a);
		if (node->value < n_small->value)
		{
			l_small = stack_a;
			n_small = ft_get_content(l_small);
		}
		stack_a = stack_a->next;
	}
	return (l_small);
}

// returns the index where val should be inserted so that stack_b stays descending
int	find_insert_pos_desc(t_list *b, int val)
{
	int		pos;
	int		len;
	t_list	*max_node;
	t_list	*cur;
	int		cur_val;
	int		next_val;

	len = ft_lstsize(b);
	if (len < 2)
		return (0);
	max_node = ft_lstmax(b); // top of wrap: max followed by min
	cur = max_node;
	pos = cur->index; // indices assumed set by ft_lstindex
	while (1)
	{
		cur_val = ft_get_content(cur)->value;
		next_val = ft_get_content(cur->next ? cur->next : b)->value;
		// insert between cur and next if val fits descending, including wrap
		if ((val <= cur_val && val >= next_val) ||                      
			// normal window
			(cur_val < next_val && (val >= cur_val || val <= next_val)))
				// wrap window
			return (pos + 1 == len ? 0 : pos + 1);
		cur = cur->next ? cur->next : b;
		pos = cur->index;
		if (cur == max_node)
			break ;
	}
	return (0);
}

void	push_with_insert(t_list **a, t_list **b)
{
	int	val;
	int	len_b;
	int	pos;

	val = ft_get_content(*a)->value;
	len_b = ft_lstsize(*b);
	pos = find_insert_pos_desc(*b, val);
	if (pos <= len_b / 2)
		while (pos--)
			ft_rb(b, 1);
	else
		while (pos++ < len_b)
			ft_rrb(b, 1);
	ft_pb(a, b, 1);
}

// Affecte a l'ensemble de la stack_b le noeud target
void	ft_affect_target(t_list **stack_a, t_list **stack_b)
{
	t_node	*n_a;
	t_node	*n_b;
	t_node	*n_target;
	t_list	*l_a;
	t_list	*l_b;
	t_list	*l_target;

	l_b = *stack_b;
	while (l_b)
	{
		n_b = ft_get_content(l_b);
		// l_target = ft_find_smallest(*stack_a);
		// n_target = ft_get_content(l_target);
		l_target = NULL;
		n_target = NULL;
		l_a = *stack_a;
		while (l_a)
		{
			n_a = ft_get_content(l_a);
			if (n_a->value > n_b->value)
			{
				if (!l_target || n_a->value < n_target->value)
				{
					l_target = l_a;
					n_target = ft_get_content(l_target);
				}
			}
			l_a = l_a->next;
		}
		if (!l_target)
			l_target = ft_find_smallest(*stack_a);
		n_b->target = l_target;
		l_b = l_b->next;
	}
	// t_list *l_current = *stack_b;
	// while (l_current)
	// {
	// 	t_node *n_current  = ft_get_content(l_current);
	// 	l_target = ft_get_target(n_current);
	// 	n_target = ft_get_content(l_target);
	// 	ft_printf("CURRENT = %d || TARGET = %p || VALUE = %d\n",
	// n_current->value, n_current->target, n_target->value);
	// 	l_current = l_current->next;
	// }
}

void	ft_check_above(t_list **stack)
{
	t_list	*l_current;
	t_node	*n_current;
	int		l_len;

	l_current = *stack;
	l_len = ft_lstsize(*stack);
	while (l_current)
	{
		n_current = ft_get_content(l_current);
		if (l_current->index < l_len / 2)
			n_current->above = 1;
		else
			n_current->above = 0;
		l_current = l_current->next;
	}
}

int	ft_sorting(t_list **stack_a, t_list **stack_b)
{
	t_list	*l_to_move;
	t_node	*min;

	l_to_move = NULL;
	ft_init(stack_a, stack_b);
	while (*stack_b)
	{
		ft_lstindex(stack_a);
		ft_lstindex(stack_b);
		ft_affect_target(stack_a, stack_b);
		ft_check_above(stack_a);
		ft_check_above(stack_b);
		ft_affect_cost(stack_a, stack_b);
		l_to_move = ft_find_cheapest(*stack_b);
		ft_move(l_to_move, stack_a, stack_b);
	}
	min = ft_node_min(*stack_a);
	while ((*stack_a)->content != min && min->above == 1)
		ft_ra(stack_a, 1);
	while ((*stack_a)->content != min && min->above == 0)
		ft_rra(stack_a, 1);
	ft_clear_all(stack_a, stack_b);
	return (0);
}
