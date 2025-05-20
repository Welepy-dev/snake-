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

typedef struct s_snake 
{
  int size;
} t_snake;

typedef struct s_map
{
  char **matrix;
  char **pos;
  int size;
} t_map;

typedef struct s_game
{
  t_snake snake;
  Vector2 window;
  t_map   map;
} t_game;

#endif
