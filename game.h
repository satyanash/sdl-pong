
#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define BALL_WIDTH 20
#define BALL_HEIGHT 20

#define PADDLE_WIDTH 200
#define PADDLE_HEIGHT 30

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

struct GameState {
	struct Paddle paddle;
	struct Ball ball;
	bool quit;
	bool gameover;
	bool paused;
	bool music;
};

#endif /* GAME_H */
