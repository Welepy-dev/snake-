#include "../includes/game.hpp"
#include <cstdlib>
#include <cstring>

void	free_matrix(char **matrix)
{
	int i;

	if (matrix) 
	{
		i = 0;
		while (matrix[i])
		{
			free(matrix[i]);
			matrix[i] = NULL;
			i++;
		}
		free(matrix);
		matrix = NULL;
	}
}

char	**init_matrix(int size)
{
	int i = -1;
	char **matrix = (char **)malloc(sizeof(char *) * (size + 1));
	if (!matrix) return NULL;

	while (++i < size)
	{
		int x = -1;
		matrix[i] = (char *)malloc(sizeof(char) * (size + 1)); 
		if (!matrix[i]) return NULL;

		while (++x < size)
		{
			if (i == 0 || i == size - 1 || x == 0 || x == size - 1)
				matrix[i][x] = '1';
			else
				matrix[i][x] = '0';
		}
		matrix[i][size] = '\0';
	}
	matrix[size] = NULL;
	int c = size / 2;
	matrix[c][c] = 'S';
	matrix[c][c + 1] = 'S';
	matrix[c][c + 2] = 'S';
	return matrix;
}

void	init(t_game *game, int ac, char *arg)
{
	game->window.x = WINDOW_SIZE;
	game->window.y = WINDOW_SIZE;
	if (ac == 2 && arg)
	{
	game->window.x = atoi(arg);
	game->window.y = atoi(arg);
	}
	game->map.size = game->window.x / 10;
	game->map.matrix = init_matrix(game->map.size);
	game->snake.size = 3;
	InitWindow(game->window.x, game->window.y, "Snake");
}

void	end_engine(t_game *game)
{
	/*game->flock.~Flock();
	UnloadRenderTexture(game->texture.hashmap);
	rlImGuiShutdown();*/

	free_matrix(game->map.matrix);
	CloseWindow();
}

void	game_loop(t_game *game)
{
  Rectangle window_borders;
  window_borders.y = CELL;
  window_borders.x = CELL;
  window_borders.height = game->window.y - (CELL * 2);
  window_borders.width = game->window.x - (CELL * 2);
	while (!WindowShouldClose()) // This returns true if ESC is pressed or window is closed
	{
		BeginDrawing();
    
    DrawRectangleRoundedLinesEx(window_borders, 0.07f, 0, 1, GREEN);

		for (int y = 0; y < game->map.size; y++) {
			for (int x = 0; x < game->map.size; x++) {
				if (game->map.matrix[y][x] == 'S')
          DrawRectangle(x * CELL + GAPS, y * CELL + GAPS, CELL - GAPS * 2, CELL - GAPS * 2, GREEN);
			}
		}
		EndDrawing();
	}
}

int	main(int ac, char **av)
{
	t_game game;

	init(&game, ac, av[1]);
	SetTargetFPS(60);

	game_loop(&game);

	end_engine(&game);
	return (0);
}

