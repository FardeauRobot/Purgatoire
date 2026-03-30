/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_free.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 22:12:15 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 13:53:49 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_strings.h"

char	*ft_strjoin_free(char *s1, char const *s2, t_select option)
{
	char	*new_str;

	new_str = ft_strjoin(s1, s2);
	if (option == FIRST || option == BOTH)
		free(s1);
	if (option == SECOND || option == BOTH)
		free((void *)s2);
	return (new_str);
}
