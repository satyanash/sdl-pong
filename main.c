// base includes
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

// SDL includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

// our headers
#include "game.h"

// custom includes
#include "util.c"
#include "sound.c"
#include "music.c"
#include "video.c"
#include "font.c"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define BALL_WIDTH 20
#define BALL_HEIGHT 20

#define PADDLE_WIDTH 200
#define PADDLE_HEIGHT 30


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

void reset_game_state(struct GameState* state){
	//init the random number generator
	srand((unsigned) time(NULL));

	bool random_bool = rand() % 1;

	state->quit = false;
	state->gameover = false;
	state->music = true;

	// paddle data
	state->paddle.width = 200;
	state->paddle.height = 30;
	state->paddle.position.x = SCREEN_WIDTH / 2;
	state->paddle.position.y = SCREEN_HEIGHT * 7/8;
	state->paddle.velocity.x = 0;
	state->paddle.velocity.y = 0;
	state->paddle.exists = true;
	state->paddle.velocityDelta = 5;

	// ball data
	state->ball.width = 20;
	state->ball.height = 20;
	state->ball.position.x = SCREEN_WIDTH / 2;
	state->ball.position.y = SCREEN_HEIGHT * 3/4;
	state->ball.velocity.x = 5 * (random_bool ? -1 : 1);
	state->ball.velocity.y = -5;
	state->ball.velocityDelta = 5;
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
	// init video
	init_video_system();
	struct VideoContext* video_ctx = load_video("Breakout Pong", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// init fonts
	init_font_system();
	struct FontContext* font_ctx = load_fonts();

	// init audio
	init_audio_system();
	struct SoundContext* sound_ctx = load_sounds();
	struct MusicContext* music_ctx = load_musics();

	// Initialize SDL Image
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
		log_SDLError(stderr, "IMG_Init");
		SDL_Quit();
		return 1;
	}


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
		render_game(&state, video_ctx, font_ctx, &screen_rect);
	}

	free_music_system(music_ctx);
	free_sound_system(sound_ctx);
	free_video_system(video_ctx);

	IMG_Quit();
	return 0;
}
