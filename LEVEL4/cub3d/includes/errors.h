/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/17 15:29:25 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_ERRORS_H
# define CUB3D_ERRORS_H

typedef enum e_err
{
	ERRN_NONE,
	ERRN_EMPTY_FILE,
	ERRN_MALLOC,
	ERRN_LOAD,
	ERRN_OPEN,
	ERRN_ARGS,
	ERRN_TEXTURES,
	ERRN_PARSING,
	ERRN_WALL,
	ERRN_UNKNOWN
}	t_err;

/* ============== ERROR MESSAGES ============================ */

# define ERR_MSG_MALLOC	"memory allocation failed"

// ========= ERRORS_PARSING ============ //

# define ERR_MSG_ARGS	    "wrong number of arguments"
# define ERR_MSG_ARGC	    "takes only : ./cub3d ./path_to_map"
# define ERR_MSG_FORMAT		"file must end in .cub"
# define ERR_MSG_INVALID_ID		"file must end in .cub"
# define ERR_MSG_INVALID_CHAR	"Invalid char found on the map"
# define ERR_MSG_PLAYER_COUNT	"Too much player located on the map"
# define ERR_MSG_WALLS		"Map isn't closed by walls"
# define ERR_MSG_PARSING	"error parsing"
# define ERR_MSG_TEXTURES	"couldn't load all textures"
# define ERR_MSG_OPEN		"cannot open file"
# define ERR_MSG_EMPTY	    "empty file"
# define ERR_MSG_LOADING	"Error while loading game"
# define ERR_MSG_MLX		"Mlx didn't initiate properly"
# define ERR_FAIL_MLX		"Something went wrong with MLX"

// ========= ERRORS_PARSING ============ //

/* ============== ERROR FUNCTIONS ========================== */
/* src/utils/error.c */
int		ft_error(char *context, char *detail, int error);
void	ft_exit(t_cub *data, int error, char *context, char *detail);
int		check_args(int argc, int expected, char *usage);

/* ============== MEMORY HELPERS =========================== */
/* src/utils/memory.c */
void	*safe_malloc(size_t size);
void	free_split(char **tab);
void	free_data(t_cub *data);

#endif
