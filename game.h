
#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// SDL Includes
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define BALL_WIDTH 20
#define BALL_HEIGHT 20

#define PADDLE_WIDTH 200
#define PADDLE_HEIGHT 30

#define MAX_TILE_ROWS 10
#define MAX_TILE_COLS 8

#define MIN_TILE_HEIGHT 20
#define MIN_TILE_WIDTH (SCREEN_WIDTH / MAX_TILE_COLS)

struct Vector {
	int x;
	int y;
};

typedef struct Vector Point;
typedef struct Vector Velocity;

struct Ball {
	int width;
	int height;
	Point position;
	Velocity velocity;
	int velocityDelta;
};

struct Paddle {
	int width;
	int height;
	Point position;
	Velocity velocity;
	bool exists;
	int velocityDelta;
};

struct Tile {
	//Point position;
	//int width;
	//int height;
	//SDL_Color color;
	bool broken;
};

struct TileMap {
	//int tile_count;
	struct Tile* tiles[MAX_TILE_ROWS][MAX_TILE_COLS];
};

struct GameState {
	struct Paddle paddle;
	struct Ball ball;
	bool quit;
	bool gameover;
	bool paused;
	bool music;
	struct TileMap* tile_map;
};

#endif /* GAME_H */
