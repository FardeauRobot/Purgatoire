/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:09:53 by tibras            #+#    #+#             */
/*   Updated: 2025/12/03 15:52:37 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int main (int argc, char **argv)
{
    t_list  *stack_a;
    t_list  *stack_b;

    stack_b = NULL;
    (void)*stack_b;
    if (ft_parsing(&stack_a, argc, argv))
        return (1);
    return (0);
}