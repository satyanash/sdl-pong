// base includes
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

// SDL includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// custom includes
#include "sound.c"
#include "music.c"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define BALL_WIDTH 20
#define BALL_HEIGHT 20

#define PADDLE_WIDTH 200
#define PADDLE_HEIGHT 30

const char* res_open_sans_ttf = "./res/OpenSans-Regular.ttf";

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
	bool paused;
	bool music;
};


SDL_Rect screen_rect = {
	.x = 0,
	.y = 0,
	.w = SCREEN_WIDTH,
	.h = SCREEN_HEIGHT,
};

void log_ball_state(const struct Ball* ball){
	printf("BALL IMPACT: %d %d\tVEL: %d %d\n", ball->position.x, ball->position.y, ball->velocity.x, ball->velocity.y);
}

void log_paddle_state(const struct Paddle* paddle){
	printf("PADDLE IMPACT: %d %d\tVEL: %d %d\n", paddle->position.x, paddle->position.y, paddle->velocity.x, paddle->velocity.y);
}

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

void reset_game_state(struct GameState* state){
	//init the random number generator
	srand((unsigned) time(NULL));

	bool random_bool = rand() % 1;

	state->quit = false;
	state->gameover = false;
	state->music = true;

	// paddle data
	state->paddle.position.x = SCREEN_WIDTH / 2;
	state->paddle.position.y = SCREEN_HEIGHT * 7/8;
	state->paddle.velocity.x = 0;
	state->paddle.velocity.y = 0;
	state->paddle.exists = true;
	state->paddle.velocityDelta = 5;

	// ball data
	state->ball.position.x = SCREEN_WIDTH / 2;
	state->ball.position.y = SCREEN_HEIGHT * 3/4;
	state->ball.velocity.x = 5 * (random_bool ? -1 : 1);
	state->ball.velocity.y = -5;
	state->ball.velocityDelta = 5;
}

void render_game( const struct GameState* state, SDL_Renderer* ren){

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
		SDL_Texture* text = renderText("GAME OVER!", res_open_sans_ttf, color, 64, ren);

		SDL_Rect dst;
		SDL_QueryTexture(text, NULL, NULL, &dst.w, &dst.h); //Query the texture to get its width and height to use
		dst.x = (SCREEN_WIDTH / 2) - dst.w / 2;
		dst.y = (SCREEN_HEIGHT / 2) - dst.h / 2;

		SDL_RenderCopy(ren, text, NULL, &dst);

		SDL_Texture* subtext = renderText("press ENTER to play again, Q to quit", res_open_sans_ttf, color, 16, ren);
		SDL_Rect subdst;
		SDL_QueryTexture(subtext, NULL, NULL, &subdst.w, &subdst.h);
		subdst.x = (SCREEN_WIDTH / 2) - subdst.w / 2;
		subdst.y = (SCREEN_HEIGHT / 2) + dst.h / 2 + 20;

		SDL_RenderCopy(ren, subtext, NULL, &subdst);
	}

	if(state->paused){
		SDL_Color color = { 255, 255, 255, 255 };
		SDL_Texture* text = renderText("PAUSED", res_open_sans_ttf, color, 64, ren);

		SDL_Rect dst;
		SDL_QueryTexture(text, NULL, NULL, &dst.w, &dst.h); //Query the texture to get its width and height to use
		dst.x = (SCREEN_WIDTH / 2) - dst.w / 2;
		dst.y = (SCREEN_HEIGHT / 2) - dst.h / 2;

		SDL_RenderCopy(ren, text, NULL, &dst);
	}


	// finally present the rendered stuff
	SDL_RenderPresent(ren);
}

void handle_event(const SDL_Event* e, struct GameState* state){
	if (e->type == SDL_QUIT || (e->key.keysym.sym == SDLK_q && e->type == SDL_KEYUP)){
		state->quit = true;
	}
	if (e->key.keysym.sym == SDLK_RETURN && e->type == SDL_KEYUP){
		reset_game_state(state);
	}
	if (e->key.keysym.sym == SDLK_p && e->type == SDL_KEYUP && !state->gameover){
		state->paused = !state->paused;
		state->music = !state->music;
	}
	if (e->key.keysym.sym == SDLK_m && e->type == SDL_KEYUP && !state->gameover){
		state->music = !state->music;
	}
	if(! state->gameover && state->paddle.exists){
		//If a key was pressed
		if( e->type == SDL_KEYDOWN && e->key.repeat == 0 ) {
			//Adjust the velocity
			switch( e->key.keysym.sym ) {
				case SDLK_UP:
					state->paddle.velocity.y -= state->paddle.velocityDelta;
					break;
				case SDLK_DOWN:
					state->paddle.velocity.y += state->paddle.velocityDelta;
					 break;
				case SDLK_LEFT:
					state->paddle.velocity.x -= state->paddle.velocityDelta;
					break;
				case SDLK_RIGHT:
					state->paddle.velocity.x += state->paddle.velocityDelta;
					 break;
			}
		}
		//If a key was released
		if( e->type == SDL_KEYUP && e->key.repeat == 0 ) {
			//Adjust the velocity
			switch( e->key.keysym.sym ) {
				case SDLK_UP:
					state->paddle.velocity.y += state->paddle.velocityDelta;
					break;
				case SDLK_DOWN:
					state->paddle.velocity.y -= state->paddle.velocityDelta;
					 break;
				case SDLK_LEFT:
					state->paddle.velocity.x += state->paddle.velocityDelta;
					break;
				case SDLK_RIGHT:
					state->paddle.velocity.x -= state->paddle.velocityDelta;
					 break;
			}
		}
	}
}

void perform_game_logic( struct GameState* state, struct SoundContext* sound_ctx, struct MusicContext* music_ctx){
	struct Ball* ball = &state->ball;
	struct Paddle* paddle = &state->paddle;
	if(!state->gameover && !state->paused){
		// change the ball position
		ball->position.x += ball->velocity.x;
		ball->position.y += ball->velocity.y;

		//If the ball went too far to the left or right
		if( ball->position.x < 0 ){
			log_ball_state(ball);
			ball->velocity.x = ball->velocityDelta;
			play_sound(sound_ctx, MIX_BOUNCE);
		}
		if( ball->position.x + BALL_WIDTH > SCREEN_WIDTH ) {
			log_ball_state(ball);
			ball->velocity.x = -ball->velocityDelta;
			play_sound(sound_ctx, MIX_BOUNCE);
		}
		if( ball->position.y < 0 ){
			log_ball_state(ball);
			ball->velocity.y = ball->velocityDelta;
			play_sound(sound_ctx, MIX_BOUNCE);
		}
		if( ball->position.y + BALL_HEIGHT > SCREEN_HEIGHT ) {
			//state.ball.velocity.y = -state.ball.velocityDelta;
			state->gameover = true;
			play_sound(sound_ctx, MIX_DEAD);
			state->music = false;
		}

		//Paddle and Ball collision code
		if(paddle->exists){
			//detect if ball hit the paddle
			if( ball->position.y + BALL_HEIGHT >= paddle->position.y && ball->position.y + BALL_HEIGHT <= paddle->position.y + PADDLE_HEIGHT){
				if(ball->position.x + BALL_WIDTH >= paddle->position.x && ball->position.x + BALL_WIDTH <= paddle->position.x + PADDLE_WIDTH){
					//increment ball and paddle velocity delta due to successful paddle hit
					//ball->velocityDelta += 1;
					//paddle->velocityDelta += 2;

					log_ball_state(ball);
					log_paddle_state(paddle);

					ball->velocity.y = -ball->velocityDelta;
					play_sound(sound_ctx, MIX_KNOCK);
				}
			}

			// change the paddle position
			paddle->position.x += paddle->velocity.x;
			paddle->position.y += paddle->velocity.y;

			//If the paddle went too far to the left or right
			if( paddle->position.x < 0){
				log_paddle_state(paddle);
				paddle->position.x += paddle->velocityDelta;
			}
			if( paddle->position.x + PADDLE_WIDTH > SCREEN_WIDTH ){
				log_paddle_state(paddle);
				paddle->position.x -= paddle->velocityDelta;
			}
			if( paddle->position.y < 0){
				log_paddle_state(paddle);
				paddle->position.y += paddle->velocityDelta;
			}
			if( paddle->position.y + PADDLE_HEIGHT > SCREEN_HEIGHT ){
				log_paddle_state(paddle);
				paddle->position.y -= paddle->velocityDelta;
			}
		}
	}

	if(state->music){
		start_music(music_ctx, MUS_ONE);
	} else {
		stop_music(music_ctx);
	}
}

int main(const int argc, const char **argv)
{

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

	init_audio_system();
	struct SoundContext* sound_ctx = load_sounds();
	struct MusicContext* music_ctx = load_musics();

	SDL_Window *win = SDL_CreateWindow("Breakout Pong", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	struct GameState state;
	reset_game_state(&state);

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
			handle_event(&e, &state);
		}

		// 2. do whatever logic needs to be done
		perform_game_logic(&state, sound_ctx, music_ctx);

		// 3. Render the scene
		render_game(&state, ren);
	}

	free_music_system(music_ctx);
	free_sound_system(sound_ctx);

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	IMG_Quit();
	return 0;
}
