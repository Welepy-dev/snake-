#ifndef GAME_HPP
# define GAME_HPP

// MACROS
#include "raylib.h"
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
const int WINDOW_SIZE = 600;
const int CELL = 10;
const int GAPS = 1;

// INCLUDES
#include "./classes/Boid.hpp"
#include "./classes/Flock.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include <cmath>
#include <cstdlib>

// DATA STRUCTURES
typedef struct s_spawn
{
	Circle circle;
	Rectangle rect;
	bool draw;
	bool active;
	bool oncollision;
} t_spawn;

typedef struct s_player
{
	Camera2D camera;
	Boid *focused_boid;
	bool focus;
	bool shifting;
} t_player;

typedef struct s_texture2d
{
	RenderTexture2D hashmap;
} t_texture2d;

// MAIN GAME ENGINE DATA STRUCTURE
/*typedef struct s_game
{
	t_player player;
	t_spawn spawn;
	t_texture2d texture;
	Color textcolor;
	Flock flock;
	double ingameseconds;
	bool pause;
	int frame_limit;
} t_game;*/

typedef struct snake_cell
{
	Vector2				dir;
	Vector2				pos;
	int					index;
	struct snake_cell	*next;
}	t_snake_cell;

typedef struct s_snake 
{
	int				size;
	t_snake_cell	head;
	t_snake_cell	*body;
}	t_snake;

typedef struct s_collectible
{
	Vector2 pos;
	bool	have;
}	Collectible;

typedef struct s_map
{
	char		**matrix;
	char		**pos;
	Rectangle	borders;
	int			size;
	Collectible	apple;
}	t_map;

typedef struct score
{
	char	score_text[32];
	int		score;
}	Score;

typedef struct s_game
{
	t_snake	snake;
	Vector2	window;
	t_map   map;
	int		frame_counter;
	int		move_delay;
	Score	score;
}	t_game;

#endif
