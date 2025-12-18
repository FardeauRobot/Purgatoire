/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lis.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:44:14 by tibras            #+#    #+#             */
/*   Updated: 2025/12/18 17:17:43 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

int	*ft_lis_init(int len, int mod)
{
	int	*lis;
	int	i;

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
	int	i;
	int	j;

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

int	*ft_make_lis(int *s_arr, int *parent, int max_len, int max_index)
{
	int	i;
	int	j;
	int	*res;

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

int	ft_free_lis(int *lis, int *par, int *s_arr)
{
	if (lis)
		free(lis);
	if (par)
		free(par);
	if (s_arr)
		free(s_arr);
	return (1);
}

int	*ft_save_lis(t_list **stack_a, int *lis_len)
{
	int	s_len;
	int	max_index;
	int	*s_arr;
	int	*lis;
	int	*parent;
	int	*result;

	max_index = 0;
	s_len = ft_lstsize(*stack_a);
	s_arr = ft_stack_to_arr(*stack_a, s_len); // Nous donne le tableau
	lis = ft_lis_init(s_len, 0);
	// Nous renvoie la structure pour calcul de lis
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
	int	i;

	i = 0;
	while (i < lis_len)
	{
		if (value == lis[i])
			return (1);
		i++;
	}
	return (0);
}
