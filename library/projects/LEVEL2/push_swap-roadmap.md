# A FAIRE

## MALLOC TRACKING

### Push_swap project malloc locations:

1. **srcs/lis.c:20** - `ft_lis_init()` 
   - `malloc(sizeof(int) * len)` - LIS array initialization
   - Protected: Line 91 in `ft_save_lis()`

2. **srcs/lis.c:63** - `ft_make_lis()`
   - `malloc(sizeof(int) * max_len)` - Result LIS array
   - Protected: Line 82 in `init.c` (`ft_init()`)

3. **srcs/init.c:24** - `ft_stack_to_arr()`
   - `malloc(sizeof(int) * size)` - Stack to array conversion
   - Protected: Line 91 in `ft_save_lis()`

4. **utils/node_utils.c:54** - `ft_new_node()`
   - `malloc(sizeof(t_node))` - Node creation
   - Protected: Need to verify caller checks

5. **includes/libft/List/ft_lstnew.c:19** - `ft_lstnew()`
   - `malloc(sizeof(t_list))` - List element creation
   - Protected: Need to verify caller checks

### Libft malloc locations (for reference):
- Conversion/ft_itoa.c:45 - String conversion
- Strings/ft_split_charset.c:54, 103 - String splitting
- Strings/ft_strdup.c:23 - String duplication
- Strings/ft_substr.c:26, 35 - Substring creation
- Strings/ft_strmapi.c:23 - String mapping
- Strings/ft_strjoin.c:23 - String joining
- Strings/ft_split_sep.c:56, 105 - String splitting with separator
- Strings/ft_strtrim.c:44 - String trimming
- Memory/ft_calloc.c:25 - Allocated zeroed memory
- Output/ft_printf_nbr.c:47 - Number to string conversion
- GNL/get_next_line.c:44 - Buffer allocation
- GNL/get_next_line_utils.c:35 - GNL string joining

---

## Les fonctions pour trouver la substring la plus grande

Nom de fonction :	int *ft_find_sub(t_list *lst)

## Nous rend la plus liste

## On envoie tout dans la stack b sauf si value == sub[i]

Nom de fonction : 	void	ft_prep(t_list *lst, int *sub)
(On n'oublie pas de free a la fin la sub)

## On fait le tri

Nom de fonction : 	void	ft_sort(t_list **stack_a, t_list **stack_b)
( Boucle dedans tant que pas trier )
{
	On affecte les coups / les medianes / les target;
	on cherche le cheapest;
	On fait remonter le cheapest et sa target en haut;
	on le push de la stack_b vers la stack_a;
}

## Comment affecter le cout

Nom de fonction : void	ft_affect_costs(t_list **stack_a, t_list **stack_b)
{
	On parcourt la stack_b
	{
		a chaque noeud on affecte son target_node;
		a chaque noeud on affecte sa mediane;
		a chaque noeud on affecte son nombre de cout vers le haut de la pile;
		on regarde le cout pour les noeuds de la stack_b 
		si to_top_noeud > to_top_target
			cost = to_top du noeud
		sinon
			cost = to_top_target
	}
}

### Affecter le target node (A REVOIR)

Nom de fonction : void	ft_affect_target(t_node *node, t_list **stack_a)
{
	t_node	*tmp;
	t_node	*next;
	int nb;

	nb = node->value;
	on parcourt la stack_a
	{
		tmp = stack_a->content;
		next = stack_a->next;
		if (tmp->value < nb && nb < next->value)
		{
			node->target = next;
			return;
		}
		stack_a = stack_a->next;
	}
	tmp = ft_lstmax();
	node->target = tmp->next;
}

### Calculer le nombre de coups pour remonter au top

Nom de fonction : int ft_to_top(t_node *node, t_list **stack)
{
	int stack_size;
	int cost;

	stack_size = ft_lstsize(stack)
	if (node->above)
		cost = node->index;
	else
		cost = stack_size - node->index;
	return (cost);
}


#### Affecter mediane > ou <

Nom de fonction : void ft_lst_med(t_list **stack)
{
	taille = On prend la taille de la liste
	On parcourt la liste
	{
		si (index < taille / 2)
			above = 1;
		else
			above = 0;
	}
}