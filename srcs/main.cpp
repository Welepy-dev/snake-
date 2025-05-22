#include "../includes/game.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

Vector2 get_apple_cordinates(char **matrix, int map_size)
{
    Vector2 cordinates = {-1, -1}; // Initialize to invalid coordinates
    for (int y = 0; y < map_size; y++)
    {
        for (int x = 0; x < map_size; x++)
        {
            if (matrix[y][x] == 'R')
            {
                cordinates.x = x;
                cordinates.y = y;
                return cordinates; // Exit immediately after finding the apple
            }
        }
    }
    return cordinates; // Return (-1, -1) if no apple is found
}

void free_snake_body(t_snake *snake)
{
    t_snake_cell *current = snake->body;
    t_snake_cell *next;

    while (current)
    {
        next = current->next;
        free(current);
        current = next;
    }
    snake->body = NULL;
}

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

t_snake_cell	*get_last_cell(t_snake_cell *cell)
{
	if (cell == NULL) return (NULL);
	while(cell->next)
		cell = cell->next;
	return (cell);
}

void add_cell(t_snake *snake)
{
    t_snake_cell *new_cell = (t_snake_cell *)malloc(sizeof(t_snake_cell));
    if (!new_cell) return; 

    new_cell->next = NULL;
    new_cell->index = 0;
    t_snake_cell *last = get_last_cell(snake->body);
    if (last)
    {
        new_cell->pos.x = last->pos.x - snake->head.dir.x;
        new_cell->pos.y = last->pos.y - snake->head.dir.y;
        last->next = new_cell;
    }
    else
    {
        new_cell->pos.x = snake->head.pos.x - snake->head.dir.x;
        new_cell->pos.y = snake->head.pos.y - snake->head.dir.y;
        snake->body = new_cell;
    }
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
	snake.body = (t_snake_cell *)malloc(sizeof(t_snake_cell));
    if (!snake.body) return (snake);
	snake.body->pos.x = initial_pos + 1; // Place body behind head (adjust based on direction)
    snake.body->pos.y = initial_pos;
    snake.body->index = 0;
    snake.body->next = NULL;
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

void move_snake(t_game *game)
{
    t_snake_cell *current = game->snake.body;
    int oldx = game->snake.head.pos.x;
    int oldy = game->snake.head.pos.y;
    int x = game->snake.head.pos.x + game->snake.head.dir.x;
    int y = game->snake.head.pos.y + game->snake.head.dir.y;

    // Handle wall wrapping
    if (game->map.matrix[y][x] == '1')
    {
        if (game->snake.head.dir.x == -1) // Left wall
            x = game->map.size - 2;
        else if (game->snake.head.dir.x == 1) // Right wall
            x = 1;
        else if (game->snake.head.dir.y == -1) // Top wall
            y = game->map.size - 2;
        else if (game->snake.head.dir.y == 1) // Bottom wall
            y = 1;
    }

    // Check for collision with body
    if (game->map.matrix[y][x] == 'S')
    {
        CloseWindow(); // End game on self-collision
        return;
    }

    // Handle apple consumption
    if (game->map.matrix[y][x] == 'R')
    {
        game->map.apple.have = false;
        game->score.score += 10;
        sprintf(game->score.score_text, "Score: %d", game->score.score);
        add_cell(&game->snake);
    }
    else
    {
        // Clear the tail only if no apple is eaten
        t_snake_cell *last = get_last_cell(game->snake.body);
        if (last)
		{
			int lastx = last->pos.x;
			int lasty = last->pos.y;
            game->map.matrix[lasty][lastx] = '0';
		}
    }

    // Move body cells
    while (current)
    {
        int temp_x = current->pos.x;
        int temp_y = current->pos.y;
        current->pos.x = oldx;
        current->pos.y = oldy;
        oldx = temp_x;
        oldy = temp_y;
        current = current->next;
    }

    // Update head
    game->map.matrix[y][x] = 'S';
    game->map.matrix[oldy][oldx] = '0';
    game->snake.head.pos.x = x;
    game->snake.head.pos.y = y;
}

void change_snake_dir(t_snake *snake, Vector2 apple_cordinates)
{
    // If apple coordinates are invalid, do nothing
    if (apple_cordinates.x < 0 || apple_cordinates.y < 0)
        return;

    // Calculate differences
    int dx = apple_cordinates.x - snake->head.pos.x;
    int dy = apple_cordinates.y - snake->head.pos.y;

    // Prioritize movement in the x-axis if the difference is larger
    if (abs(dx) > abs(dy))
    {
        if (dx > 0 && snake->head.dir.x != -1) // Move right, unless going left
        {
            snake->head.dir.x = 1;
            snake->head.dir.y = 0;
            printf("Moving right\n");
        }
        else if (dx < 0 && snake->head.dir.x != 1) // Move left, unless going right
        {
            snake->head.dir.x = -1;
            snake->head.dir.y = 0;
            printf("Moving left\n");
        }
    }
    else // Prioritize y-axis or equal distance
    {
        if (dy > 0 && snake->head.dir.y != -1) // Move down, unless going up
        {
            snake->head.dir.x = 0;
            snake->head.dir.y = 1;
            printf("Moving down\n");
        }
        else if (dy < 0 && snake->head.dir.y != 1) // Move up, unless going down
        {
            snake->head.dir.x = 0;
            snake->head.dir.y = -1;
            printf("Moving up\n");
        }
    }
}

void	end_engine(t_game *game)
{
	free_snake_body(&game->snake);
	free_matrix(game->map.matrix);
	CloseWindow();
}

void game_loop(t_game *game)
{
    while (!WindowShouldClose())
    {
        change_snake_dir(&game->snake, get_apple_cordinates(game->map.matrix, game->map.size));

        game->frame_counter++;
        if (game->frame_counter >= game->move_delay)
        {
            if (game->map.apple.have == false)
                place_aplle(&game->map);
            move_snake(game);
            // Update matrix with body positions
            t_snake_cell *current = game->snake.body;
            while (current)
            {
				int currenty = current->pos.y;
				int currentx = current->pos.x;
                game->map.matrix[currenty][currentx] = 'S';
                current = current->next;
            }
            game->frame_counter = 0;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw snake head
        DrawRectangle(game->snake.head.pos.x * CELL + GAPS, game->snake.head.pos.y * CELL + GAPS,
                      CELL - GAPS * 2, CELL - GAPS * 2, GREEN);

        // Draw snake body
        t_snake_cell *current = game->snake.body;
        while (current)
        {
            DrawRectangle(current->pos.x * CELL + GAPS, current->pos.y * CELL + GAPS,
                          CELL - GAPS * 2, CELL - GAPS * 2, GREEN);
            current = current->next;
        }

        // Draw apple
        for (int y = 0; y < game->map.size; y++) {
            for (int x = 0; x < game->map.size; x++) {
                if (game->map.matrix[y][x] == 'R')
                    DrawRectangle(x * CELL + GAPS, y * CELL + GAPS, CELL - GAPS * 2, CELL - GAPS * 2, RED);
            }
        }

        DrawRectangleRoundedLinesEx(game->map.borders, 0.07f, 0, 1, GREEN);
        DrawText(game->score.score_text, 20, 20, 20, RAYWHITE);

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

/*
* 
#include "../includes/game.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>

struct Vector2i {
    int x, y;
};

struct SnakeCell {
    Vector2i pos;
    std::unique_ptr<SnakeCell> next;
};

class Snake {
public:
    Vector2i headPos;
    Vector2i direction = {-1, 0};
    std::unique_ptr<SnakeCell> body;

    void addCell() {
        auto newCell = std::make_unique<SnakeCell>();
        if (!newCell) return;

        SnakeCell* last = getLastCell();
        if (last) {
            newCell->pos = { last->pos.x - direction.x, last->pos.y - direction.y };
            last->next = std::move(newCell);
        } else {
            newCell->pos = { headPos.x - direction.x, headPos.y - direction.y };
            body = std::move(newCell);
        }
    }

    SnakeCell* getLastCell() {
        SnakeCell* curr = body.get();
        while (curr && curr->next) {
            curr = curr->next.get();
        }
        return curr;
    }

    void moveBody(int oldX, int oldY) {
        SnakeCell* curr = body.get();
        while (curr) {
            int tmpX = curr->pos.x;
            int tmpY = curr->pos.y;
            curr->pos = {oldX, oldY};
            oldX = tmpX;
            oldY = tmpY;
            curr = curr->next.get();
        }
    }

    void draw(int mapSize) {
        DrawRectangle(headPos.x * CELL + GAPS, headPos.y * CELL + GAPS,
                      CELL - GAPS * 2, CELL - GAPS * 2, GREEN);
        SnakeCell* curr = body.get();
        while (curr) {
            if (curr->pos.x >= 0 && curr->pos.x < mapSize &&
                curr->pos.y >= 0 && curr->pos.y < mapSize) {
                DrawRectangle(curr->pos.x * CELL + GAPS, curr->pos.y * CELL + GAPS,
                              CELL - GAPS * 2, CELL - GAPS * 2, GREEN);
            }
            curr = curr->next.get();
        }
    }
};

class Map {
public:
    int size;
    std::vector<std::vector<char>> matrix;
    Rectangle borders;
    bool appleExists = false;

    Map(int windowSize) {
        size = windowSize / CELL;
        matrix.resize(size, std::vector<char>(size, '0'));
        for (int i = 0; i < size; ++i) {
            matrix[0][i] = '1';
            matrix[size - 1][i] = '1';
            matrix[i][0] = '1';
            matrix[i][size - 1] = '1';
        }
        borders = {CELL, CELL, static_cast<float>(windowSize - 2 * CELL), static_cast<float>(windowSize - 2 * CELL)};
    }

    void placeApple() {
        appleExists = true;
        int x, y;
        do {
            x = GetRandomValue(1, size - 2);
            y = GetRandomValue(1, size - 2);
        } while (matrix[y][x] != '0');
        matrix[y][x] = 'R';
    }

    Vector2i getAppleCoordinates() const {
        for (int y = 0; y < size; ++y)
            for (int x = 0; x < size; ++x)
                if (matrix[y][x] == 'R')
                    return {x, y};
        return {-1, -1};
    }

    void draw() const {
        for (int y = 0; y < size; ++y)
            for (int x = 0; x < size; ++x)
                if (matrix[y][x] == 'R')
                    DrawRectangle(x * CELL + GAPS, y * CELL + GAPS,
                                  CELL - GAPS * 2, CELL - GAPS * 2, RED);
        DrawRectangleRoundedLinesEx(borders, 0.07f, 0, 1, GREEN);
    }
};

class Game {
public:
    int score = 0;
    int moveDelay = 5;
    int frameCounter = 0;
    int windowSize = WINDOW_SIZE;
    Snake snake;
    Map map;
    std::string scoreText;

    Game(int winSize = WINDOW_SIZE) : windowSize(winSize), map(winSize) {
        InitWindow(windowSize, windowSize, "Snake");
        SetTargetFPS(60);
        reset();
    }

    ~Game() {
        CloseWindow();
    }

    void reset() {
        score = 0;
        scoreText = "Score: 0";
        map.placeApple();
        int mid = map.size / 2;
        snake.headPos = {mid, mid};
        snake.direction = {-1, 0};
        snake.body = nullptr;
        snake.addCell();
    }

    void update() {
        if (!map.appleExists) {
            map.placeApple();
        }

        Vector2i apple = map.getAppleCoordinates();
        changeDirection(apple);

        if (++frameCounter >= moveDelay) {
            moveSnake();
            frameCounter = 0;
        }
    }

    void moveSnake() {
        int newX = snake.headPos.x + snake.direction.x;
        int newY = snake.headPos.y + snake.direction.y;

        if (newX < 0 || newX >= map.size || newY < 0 || newY >= map.size) return;

        char cell = map.matrix[newY][newX];

        // Portal wrap
        if (cell == '1') {
            if (snake.direction.x == -1) newX = map.size - 2;
            else if (snake.direction.x == 1) newX = 1;
            else if (snake.direction.y == -1) newY = map.size - 2;
            else if (snake.direction.y == 1) newY = 1;
        }

        if (map.matrix[newY][newX] == 'S') {
            CloseWindow(); // Game over
            return;
        }

        int oldX = snake.headPos.x;
        int oldY = snake.headPos.y;

        if (cell == 'R') {
            score += 10;
            scoreText = "Score: " + std::to_string(score);
            map.appleExists = false;
            snake.addCell();
        } else {
            SnakeCell* last = snake.getLastCell();
            if (last) {
                map.matrix[last->pos.y][last->pos.x] = '0';
            }
        }

        snake.moveBody(oldX, oldY);
        snake.headPos = {newX, newY};
        map.matrix[newY][newX] = 'S';

        SnakeCell* curr = snake.body.get();
        while (curr) {
            if (curr->pos.x >= 0 && curr->pos.x < map.size &&
                curr->pos.y >= 0 && curr->pos.y < map.size)
                map.matrix[curr->pos.y][curr->pos.x] = 'S';
            curr = curr->next.get();
        }
    }

    void changeDirection(const Vector2i& apple) {
        if (apple.x == -1) return;

        int dx = apple.x - snake.headPos.x;
        int dy = apple.y - snake.headPos.y;

        if (abs(dx) > abs(dy)) {
            if (dx > 0 && snake.direction.x != -1) snake.direction = {1, 0};
            else if (dx < 0 && snake.direction.x != 1) snake.direction = {-1, 0};
        } else {
            if (dy > 0 && snake.direction.y != -1) snake.direction = {0, 1};
            else if (dy < 0 && snake.direction.y != 1) snake.direction = {0, -1};
        }
    }

    void draw() {
        BeginDrawing();
        ClearBackground(BLACK);
        snake.draw(map.size);
        map.draw();
        DrawText(scoreText.c_str(), 20, 20, 20, RAYWHITE);
        EndDrawing();
    }

    void loop() {
        while (!WindowShouldClose()) {
            update();
            draw();
        }
    }
};

int main(int argc, char* argv[]) {
    int size = (argc >= 2) ? std::atoi(argv[1]) : WINDOW_SIZE;
    Game game(size);
    game.loop();
    return 0;
}
* */
