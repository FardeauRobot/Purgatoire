/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/05 12:29:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GC_H
# define FT_GC_H

# include <stdlib.h>

# include "ft_list.h"

void	*ft_gc_malloc(size_t size, t_list **gc_head);
int		ft_gc_add_node(t_list **gc_head, void *ptr);
void	ft_gc_free_all(t_list **gc_head);

char	*ft_strdup_gc(const char *str, t_list **gc_head);
char	*ft_strndup_gc(const char *s, size_t n, t_list **gc_head);
char	*ft_substr_gc(char const *s, unsigned int start, size_t len, t_list **gc_head);
char	*ft_strjoin_gc(const char *s1, const char *s2, t_list **gc_head);
char	*ft_strtrim_gc(char const *s1, char const *set, t_list **gc_head);
char	*ft_strmapi_gc(char const *s, char (*f)(unsigned int, char), t_list **gc_head);
char	**ft_split_charset_gc(char const *s, char *charset, t_list **gc_head);
char	**ft_split_sep_gc(char const *s, char sep, t_list **gc_head);
char	*ft_itoa_gc(int n, t_list **gc_head);
void	*ft_calloc_gc(size_t count, size_t size, t_list **gc_head);
t_list	*ft_lstnew_gc(void *content, t_list **gc_head);
t_list	*ft_lstmap_gc(t_list *lst, void *(*f)(void *), void (*del)(void *), t_list **gc_head);

#endif
