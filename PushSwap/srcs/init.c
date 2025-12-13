/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 15:14:30 by tibras            #+#    #+#             */
/*   Updated: 2025/12/13 18:24:35 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

/*
	Pour initialiser il nous faut :

	Connaitre la lis;
	Envoyer tous les nombres qui ne sont pas dans la LIS dans la stack_b


	Pour connaitre la LIS :
	On crée un tableau qui récupere les valeurs de la liste;
	On crée un tableau de structures t_lis : 
		index,
		size,
		valeurs[];
	On fait passer une fonction find_lis a partir de chaque element (i, arr_stack, t_lis[i]);
	{
		on prend l'index;
		on prend la taille de la lis;
		on prend les valeurs de la lis
	}
*/

int		*ft_stack_to_arr(t_list *stack, int size)
{
	int		*arr;
	t_node	*node;
	int		i;

	arr = malloc(sizeof(int) * size);
	if (!arr)
		return (NULL);
	i = 0;
	while (stack)
	{
		node = stack->content;
		arr[i] = node->value;
		i++;
		stack = stack->next;
	}
	return (arr);
}

int	*ft_lis_init(int len, int mod)
{
	int 	*lis;
	int 	i;

	lis = malloc(sizeof(int) * len);
	if (!lis)
		return (NULL);
	i = 0;
	while (i < len)
	{
		if (mod)
			lis[i] = -1;
		else
			lis[i] = 1;
		i++;
	}
	return (lis);
}

void	ft_compute_lis(int *lis, int *parent, int *arr, int s_len)
{
	int i;
	int j;

	i = 1;
	while (i < s_len)
	{
		j = 0;	
		while (j < i)
		{
			if (arr[j] < arr[i] && lis[j] + 1 > lis[i])
			{
				lis[i] = lis[j] + 1;
				parent[i] = j;
			}
			j++;
		}
		i++;
	}
}

void	ft_max_lis(int lis[], int len, int *max_len, int *max_index)
{
	int	i;

	i = 0;
	while (i < len)
	{
		if (lis[i] > *max_len)
		{
			*max_len = lis[i];
			*max_index = i;
		}
		i++;
	}
}

int *ft_make_lis(int *s_arr, int *parent, int max_len, int max_index)
{
	int i;
	int j;
	int *res;

	res = malloc(sizeof(int) * max_len);
	if (!res)
		return (NULL);
	i = max_len - 1;
	j = max_index;
	while (j > -1)
	{
		res[i] = s_arr[j];
		i--;
		j = parent[j];
	}
	return (res);
}

void	ft_free_lis(int *lis, int *par, int *s_arr)
{
	if (lis)
		free(lis);
	if (par)
		free(par);
	if (s_arr)
		free(s_arr);
}
int	*ft_save_lis(t_list **stack_a, int *lis_len)
{
	int	s_len;
	int	max_index;
	int	*s_arr;
	int	*lis;
	int	*parent;
	int *result;


	max_index = 0;
	s_len = ft_lstsize(*stack_a);
	s_arr = ft_stack_to_arr(*stack_a, s_len); // Nous donne le tableau
	lis = ft_lis_init(s_len, 0); // Nous renvoie la structure pour calcul de lis
	parent = ft_lis_init(s_len, 1);
	if (!s_arr || !lis || !parent)
	{
		ft_free_lis(lis, parent, s_arr);
		exit(ft_error_stacks(stack_a, NULL));
	}
	ft_compute_lis(lis, parent, s_arr, s_len);
	ft_max_lis(lis, s_len, lis_len, &max_index);
	result = ft_make_lis(s_arr, parent, *lis_len, max_index);
	// if (!result)
		// PROTECT
	ft_free_lis(lis, parent, s_arr);
	return (result);
}

int	ft_is_in_lis(int *lis, int lis_len, int value)
{
	int i;

	i = 0;
	while (i < lis_len)
	{
		if (value == lis[i])
			return (1);
		i++;
	}
	return (0);
}

void	ft_dispatch(t_list **stack_a, t_list **stack_b, int *lis, int lis_len)
{
	t_node	*n_current;
	int s_len;
	int i;

	i = 0;
	s_len = ft_lstsize(*stack_a);
	ft_printf("LIS = \n");
	ft_print_int_arr(lis, lis_len);
	while (i < s_len)
	{
		n_current = (*stack_a)->content;
		if (ft_is_in_lis(lis, lis_len, n_current->value))
		{
			i++;
			ft_ra(stack_a, 1);
		}
		else
			ft_pb(stack_a, stack_b, 1);
	}
}
void	ft_init(t_list **stack_a, t_list **stack_b)
{
	int	*lis;
	int lis_len;

	lis_len = 0;
	lis = ft_save_lis(stack_a, &lis_len);
	ft_dispatch(stack_a, stack_b, lis, lis_len);
	free(lis);
}