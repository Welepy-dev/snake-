#include "../includes/game.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

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
	return matrix;
}

void	place_aplle(t_map *map)
{
	map->apple.have = true;
	int randx = GetRandomValue(0, map->size);
	int randy = GetRandomValue(0, map->size);
	while (map->matrix[randy][randx] != '0')
	{
		randx = GetRandomValue(0, map->size);
		randy = GetRandomValue(0, map->size);
	}
	map->matrix[randy][randx] = 'R';
}

t_snake	init_snake(t_game *game)
{
	t_snake snake;
	int		initial_pos;

	initial_pos = game->map.size / 2;

	snake.head.pos.x = initial_pos;
	snake.head.pos.y = initial_pos;
	snake.head.dir.x = -1;
	snake.head.dir.y = 0;
	return (snake);
}

void	init(t_game *game, int ac, char *arg)
{
	game->window.x = WINDOW_SIZE;
	game->window.y = WINDOW_SIZE;
	game->score.score = 0;
	sprintf(game->score.score_text, "Score: %d", game->score.score);
	if (ac == 2 && arg)
	{
		game->window.x = atoi(arg);
		game->window.y = atoi(arg);
	}
	game->map.size = game->window.x / 10;
	game->map.matrix = init_matrix(game->map.size);
	game->map.borders.y = CELL;
	game->map.borders.x = CELL;
	game->map.borders.height = game->window.y - (CELL * 2);
	game->map.borders.width = game->window.x - (CELL * 2);
	game->frame_counter = 0;
	game->move_delay = 5;
	place_aplle(&game->map);
	game->snake = init_snake(game);
	InitWindow(game->window.x, game->window.y, "Snake");
}

void	move_snake(t_game *game)
{
	int oldx = game->snake.head.pos.x;
	int oldy = game->snake.head.pos.y;
	int x = game->snake.head.pos.x + game->snake.head.dir.x;
	int y = game->snake.head.pos.y + game->snake.head.dir.y;

	if (game->map.matrix[y][x] == 'S')
		return ;
	if (game->map.matrix[y][x] == '1')
	{
		if (game->snake.head.dir.x == -1)
			x += game->map.size -2;
		else if (game->snake.head.dir.x == 1)
			x -= game->map.size -2;
		else if (game->snake.head.dir.y == 1)
			y -= game->map.size -2;
		else if (game->snake.head.dir.y== -1)
			y += game->map.size -2;
	}
	if (game->map.matrix[y][x] == 'R')
	{
		game->map.apple.have = false;
		game->score.score += 10;
		sprintf(game->score.score_text, "Score: %d", game->score.score);
	}
	game->map.matrix[y][x] = 'S';
	game->map.matrix[oldy][oldx] = '0';
	game->snake.head.pos.x = x;
	game->snake.head.pos.y = y;
}

void	change_snake_dir(t_snake *snake)
{
	if (IsKeyPressed(KEY_UP) && (snake->head.dir.y != 1 && snake->head.dir.y != -1))
	{
		snake->head.dir.x = 0;
		snake->head.dir.y = -1;
	}
	if (IsKeyPressed(KEY_DOWN) && (snake->head.dir.y != 1 && snake->head.dir.y != -1))
	{
		snake->head.dir.x = 0;
		snake->head.dir.y = 1;
	}
	if (IsKeyPressed(KEY_RIGHT) && (snake->head.dir.x != 1 && snake->head.dir.x != -1))
	{
		snake->head.dir.x = 1;
		snake->head.dir.y = 0;
	}
	if (IsKeyPressed(KEY_LEFT) && (snake->head.dir.x != 1 && snake->head.dir.x != -1))
	{
		snake->head.dir.x = -1;
		snake->head.dir.y = 0;
	}
}

void	end_engine(t_game *game)
{
	/*game->flock.~Flock();
	UnloadRenderTexture(game->texture.hashmap);
	rlImGuiShutdown();*/

	free_matrix(game->map.matrix);
	CloseWindow();
}

void game_loop(t_game *game)
{
    while (!WindowShouldClose())
    {
        change_snake_dir(&game->snake);

        game->frame_counter++;
        if (game->frame_counter >= game->move_delay)
        {
            if (game->map.apple.have == false)
                place_aplle(&game->map);
            move_snake(game);
            game->frame_counter = 0;
        }

        BeginDrawing();
        DrawText(game->score.score_text, 20, 20, 20, RAYWHITE);
        ClearBackground(BLACK); 

        for (int y = 0; y < game->map.size; y++) {
            for (int x = 0; x < game->map.size; x++) {
                if (game->map.matrix[y][x] == 'S')
                    DrawRectangle(x * CELL + GAPS, y * CELL + GAPS, CELL - GAPS * 2, CELL - GAPS * 2, GREEN);
                else if (game->map.matrix[y][x] == 'R')
                    DrawRectangle(x * CELL + GAPS, y * CELL + GAPS, CELL - GAPS * 2, CELL - GAPS * 2, RED);
            }
        }

        DrawRectangleRoundedLinesEx(game->map.borders, 0.07f, 0, 1, GREEN);


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

