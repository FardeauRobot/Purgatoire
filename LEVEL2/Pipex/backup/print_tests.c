#include "pipex_srcs.h"

void	ft_print_char_arr(char **arr)
{
	int i  = 0;

	while (arr[i])
	{
		ft_printf("%s\n", arr[i]);
		i++;
	}
}