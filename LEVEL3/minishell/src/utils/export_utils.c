/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 09:03:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// AJOUTE LE NOEUD ENV A LA LISTE (generic ft_lstadd_back)
int	ft_env_add(t_minishell *minishell, t_env *new_env)
{
	if (!minishell || !new_env)
		return (GENERAL_ERROR);
	ft_lstadd_back((t_list **)&minishell->head_env, (t_list *)new_env);
	return (SUCCESS);
}

static void	ft_export_arr_qsort(t_env **arr, int env_size)
{
	int		index;
	t_env	*tmp;

	index = 0;
	while (index < env_size - 1)
	{
		if (ft_strcmp(arr[index]->name, arr[index + 1]->name) > 0)
		{
			tmp = arr[index];
			arr[index] = arr[index + 1];
			arr[index + 1] = tmp;
			index = -1;
		}
		index++;
	}
}

t_env	**ft_export_duplicate( t_minishell *minishell, int env_size)
{
	t_env	**arr;
	t_env	*head_env;
	int		index;

	env_size = ft_lstsize((t_list *)minishell->head_env);
	head_env = minishell->head_env;
	arr = ft_calloc_gc(env_size, sizeof(t_env *), &minishell->gc_tmp);
	if (!arr)
		return (NULL);
	index = 0;
	while (head_env)
	{
		arr[index] = head_env;
		index++;
		head_env = (t_env *)head_env->node.next;
	}
	return (arr);
}

// ON DOIT TRIER UNE LISTE CHAINEE PAR ORDRE ALPHABETIQUE EN FONCTION DE KEY
void	ft_export_null(t_minishell *minishell, int fd)
{
	int		env_size;
	t_env	**arr;

	if (!minishell->head_env)
		return ;
	env_size = ft_lstsize((t_list *)minishell->head_env);
	if (env_size < 1)
		return ;
	arr = ft_export_duplicate(minishell, env_size);
	if (!arr)
	{
		ft_error(NULL, MALLOC_FAIL, "Error malloc creating env arr", NULL);
		return ;
	}
	ft_export_arr_qsort(arr, env_size);
	ft_export_print(arr, env_size, fd);
}
