int	ft_affect_pos(t_game *game, int direction, int pos_x, int pos_y )
{
	if (direction == UP)
		pos_y--;
	if (direction == DOWN)
		pos_y++;
	if (direction == LEFT)
	{
		pos_x--;
		game->orient = O_LEFT;
	}
	if (direction == RIGHT)
	{
		pos_x++;
		game->orient = O_RIGHT;
	}
	return (ft_check_move(game, pos_x, pos_y));
}