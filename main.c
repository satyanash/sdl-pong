#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define BALL_WIDTH 20
#define BALL_HEIGHT 20

#define PADDLE_WIDTH 200
#define PADDLE_HEIGHT 30


void log_SDLError(FILE* outputfile, char* message){
	if(message != NULL){
		fprintf(outputfile, "ERROR: %s: ", message);
	}
	fprintf(outputfile, "error: %s\n", SDL_GetError());
}

struct Vector {
	int x;
	int y;
};

typedef struct Vector Point;
typedef struct Vector Velocity;

struct Ball {
	Point position;
	Velocity velocity;
	int velocityDelta;
};

struct Paddle {
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
};


SDL_Rect screen_rect = {
	.x = 0,
	.y = 0,
	.w = SCREEN_WIDTH,
	.h = SCREEN_HEIGHT,
};

SDL_Texture* renderText(const char* message, const char* fontFile, SDL_Color color, int fontSize, SDL_Renderer *renderer)
{
	//Open the font
	TTF_Font *font = TTF_OpenFont(fontFile, fontSize);
	if (font == NULL){
		log_SDLError(stderr, "TTF_OpenFont");
		return NULL;
	}

	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, message, color);
	if (surf == NULL){
		TTF_CloseFont(font);
		log_SDLError(stderr, "TTF_RenderText");
		return NULL;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL){
		log_SDLError(stderr, "CreateTexture");
	}
	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
}

struct GameState new_game_state(){
	bool random_bool = rand() % 1;

	struct GameState state = {
		.quit = false,
		.gameover = false,
		.paddle = {
			.position = { SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7/8},
			.velocity = { 0, 0 },
			.exists = true,
			.velocityDelta = 5
		},
		.ball = {
			.position = { SCREEN_WIDTH / 2, SCREEN_HEIGHT * 3/4},
			.velocity = { 5 * (random_bool ? -1 : 1) , -5 },
			.velocityDelta = 5
		},
	};
	return state;
}

void render_game( const struct GameState* state, SDL_Renderer* ren){
	//SDL_RenderClear(ren);

	//SDL_SetRenderDrawColor(ren, 97, 132, 247, SDL_ALPHA_OPAQUE);
	SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(ren, &screen_rect);

	SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE);

	if(state->paddle.exists){
		// draw the paddle
		SDL_Rect paddle;
		paddle.x = state->paddle.position.x;
		paddle.y = state->paddle.position.y;
		paddle.w = PADDLE_WIDTH;
		paddle.h = PADDLE_HEIGHT;

		SDL_RenderFillRect(ren, &paddle);
	}

	// draw the ball
	SDL_Rect ball;
	ball.x = state->ball.position.x;
	ball.y = state->ball.position.y;
	ball.w = BALL_WIDTH;
	ball.h = BALL_HEIGHT;

	SDL_RenderFillRect(ren, &ball);
	if(state->gameover){
		SDL_Color color = { 255, 255, 255, 255 };
		SDL_Texture* text = renderText("GAME OVER!", "./res/OpenSans-Regular.ttf", color, 64, ren);

		SDL_Rect dst;
		SDL_QueryTexture(text, NULL, NULL, &dst.w, &dst.h); //Query the texture to get its width and height to use
		dst.x = (SCREEN_WIDTH / 2) - dst.w / 2;
		dst.y = (SCREEN_HEIGHT / 2) - dst.h / 2;

		SDL_RenderCopy(ren, text, NULL, &dst);

		SDL_Texture* subtext = renderText("press ENTER to play again, Q to quit", "./res/OpenSans-Regular.ttf", color, 16, ren);
		SDL_Rect subdst;
		SDL_QueryTexture(subtext, NULL, NULL, &subdst.w, &subdst.h);
		subdst.x = (SCREEN_WIDTH / 2) - subdst.w / 2;
		subdst.y = (SCREEN_HEIGHT / 2) + dst.h / 2 + 20;

		SDL_RenderCopy(ren, subtext, NULL, &subdst);
	}

	// finally present the rendered stuff
	SDL_RenderPresent(ren);
}

int main(const int argc, const char **argv)
{
	//init the random number generator
	srand((unsigned) time(NULL));

	// Initialize SDL Video
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		log_SDLError(stderr, "SDL_Init");
		return 1;
	} 

	// Initialize SDL Image
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
		log_SDLError(stderr, "IMG_Init");
		SDL_Quit();
		return 1;
	}

	// Initialize SDL TTF support
	if (TTF_Init() != 0){
		log_SDLError(stderr, "TTF_Init");
		SDL_Quit();
		return 1;
	}

	// Initialize the SDL sound system
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 1, 4096 ) == -1 ) {
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Pong", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	Mix_Chunk* dead = Mix_LoadWAV("./res/dead.wav");
	Mix_Chunk* knock = Mix_LoadWAV("./res/knock.wav");
	Mix_Chunk* bounce = Mix_LoadWAV("./res/bounce.wav");

	struct GameState state = new_game_state();

	SDL_Event e;
	while (!state.quit){
		/**
		 * General event loop is as follows:
		 * 1. Events
		 * 2. Logic
		 * 3. Rendering
		 */

		// 1. handle all Events in the queue, mutate state accordingly 
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT || (e.key.keysym.sym == SDLK_q && e.type == SDL_KEYUP)){
				state.quit = true;
			}
			if (e.key.keysym.sym == SDLK_RETURN && e.type == SDL_KEYUP){
				state = new_game_state();
			}
			if(! state.gameover && state.paddle.exists){
				//If a key was pressed 
				if( e.type == SDL_KEYDOWN && e.key.repeat == 0 ) {
					//Adjust the velocity 
					switch( e.key.keysym.sym ) {
						case SDLK_LEFT:
							state.paddle.velocity.x -= state.paddle.velocityDelta;
							break;
						case SDLK_RIGHT:
							state.paddle.velocity.x += state.paddle.velocityDelta;
							 break;
					}
				}
				//If a key was released 
				if( e.type == SDL_KEYUP && e.key.repeat == 0 ) {
					//Adjust the velocity 
					switch( e.key.keysym.sym ) {
						case SDLK_LEFT:
							state.paddle.velocity.x += state.paddle.velocityDelta;
							break;
						case SDLK_RIGHT:
							state.paddle.velocity.x -= state.paddle.velocityDelta;
							 break;
					}
				}
			}
		}

		// 2. do whatever logic needs to be done

		if(!state.gameover){
			// change the ball position
			state.ball.position.x += state.ball.velocity.x;
			state.ball.position.y += state.ball.velocity.y;

			//If the ball went too far to the left or right 
			if( state.ball.position.x < 0 ){
				printf("BALL IMPACT: %d %d\tVEL: %d %d\n", state.ball.position.x, state.ball.position.y, state.ball.velocity.x, state.ball.velocity.y);
				state.ball.velocity.x = state.ball.velocityDelta;
				Mix_PlayChannel(-1, bounce, 0);
			}
			if( state.ball.position.x + BALL_WIDTH > SCREEN_WIDTH ) {
				printf("BALL IMPACT: %d %d\tVEL: %d %d\n", state.ball.position.x, state.ball.position.y, state.ball.velocity.x, state.ball.velocity.y);
				state.ball.velocity.x = -state.ball.velocityDelta;
				Mix_PlayChannel(-1, bounce, 0);
			}
			if( state.ball.position.y < 0 ){
				printf("BALL IMPACT: %d %d\tVEL: %d %d\n", state.ball.position.x, state.ball.position.y, state.ball.velocity.x, state.ball.velocity.y);
				state.ball.velocity.y = state.ball.velocityDelta;
				Mix_PlayChannel(-1, bounce, 0);
			}
			if( state.ball.position.y + BALL_HEIGHT > SCREEN_HEIGHT ) {
				//printf("BALL IMPACT: %d %d\tVEL: %d %d\n", state.ball.position.x, state.ball.position.y, state.ball.velocity.x, state.ball.velocity.y);
				//state.ball.velocity.y = -state.ball.velocityDelta;
				state.gameover = true;
				Mix_PlayChannel(-1, dead, 0);
			}

			if(state.paddle.exists){
				//detect if ball hit the paddle
				if( state.ball.position.y + BALL_HEIGHT >= state.paddle.position.y && state.ball.position.y + BALL_HEIGHT <= state.paddle.position.y + PADDLE_HEIGHT){
					if(state.ball.position.x + BALL_WIDTH >= state.paddle.position.x && state.ball.position.x + BALL_WIDTH <= state.paddle.position.x + PADDLE_WIDTH){
						//increment ball and paddle velocity delta due to successful paddle hit
						state.ball.velocityDelta += 1;
						state.paddle.velocityDelta += 2;

						printf("BALL IMPACT: %d %d\tVEL: %d %d\n", state.ball.position.x, state.ball.position.y, state.ball.velocity.x, state.ball.velocity.y);
						printf("PADDLE: %d %d\tVEL: %d %d\n", state.paddle.position.x, state.paddle.position.y, state.paddle.velocity.x, state.paddle.velocity.y);
						state.ball.velocity.y = -state.ball.velocityDelta;
						Mix_PlayChannel(-1, knock, 0);
					}
				}

				// change the paddle position
				state.paddle.position.x += state.paddle.velocity.x;
				state.paddle.position.y += state.paddle.velocity.y;

				//If the paddle went too far to the left or right 
				if( state.paddle.position.x < 0){
					printf("PADDLE IMPACT: %d %d\n", state.ball.position.x, state.ball.position.y);
					state.paddle.position.x += state.paddle.velocityDelta;
				}
				if( state.paddle.position.x + PADDLE_WIDTH > SCREEN_WIDTH ){
					printf("PADDLE IMPACT: %d %d\n", state.ball.position.x, state.ball.position.y);
					state.paddle.position.x -= state.paddle.velocityDelta;
				}
			}
		}

		// 3. Render the scene
		render_game(&state, ren);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	IMG_Quit();
	return 0;
}
