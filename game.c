#ifndef GAME_C
#define GAME_C

#include <time.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "game.h"

#include "music.c"
#include "sound.c"
#include "levels.c"
#include "util.c"

void log_ball_state(const struct Ball* ball){
	printf("BALL IMPACT: %d %d\tVEL: %d %d\n", ball->position.x, ball->position.y, ball->velocity.x, ball->velocity.y);
}

void log_paddle_state(const struct Paddle* paddle){
	printf("PADDLE IMPACT: %d %d\tVEL: %d %d\n", paddle->position.x, paddle->position.y, paddle->velocity.x, paddle->velocity.y);
}

void reset_game_state(struct GameState* state, int level){
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

	//level
	printf("MIN_TILE_WIDTH: %d\n", MIN_TILE_WIDTH);
	printf("MIN_TILE_HEIGHT: %d\n", MIN_TILE_HEIGHT);
	printf("MAX_TILE_ROWS: %d\n", MAX_TILE_ROWS);
	printf("MAX_TILE_COLS: %d\n", MAX_TILE_COLS);
	state->tile_map = get_level(level);
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

		SDL_Rect ball_rect;
		ball_rect.x = ball->position.x;
		ball_rect.y = ball->position.y;
		ball_rect.w = ball->width;
		ball_rect.h = ball->height;

		SDL_Rect tile_rect;

		//Paddle and Tile collision code
		for(int i = 0; i < MAX_TILE_ROWS; i++){
			for(int j = 0; j < MAX_TILE_COLS; j++){
				if( ! state->tile_map->tiles[i][j]->broken){
					tile_rect.x = j * MIN_TILE_WIDTH;
					tile_rect.y = i * MIN_TILE_HEIGHT;
					tile_rect.w = MIN_TILE_WIDTH;
					tile_rect.h = MIN_TILE_HEIGHT;

					if(intersects(&ball_rect, &tile_rect)){
						state->tile_map->tiles[i][j]->broken = true;
						ball->velocity.y = -ball->velocity.y;
					}
				}
			}
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

void handle_event(const SDL_Event* e, struct GameState* state){
	if (e->type == SDL_QUIT || (e->key.keysym.sym == SDLK_q && e->type == SDL_KEYUP)){
		state->quit = true;
	}
	if (e->key.keysym.sym == SDLK_RETURN && e->type == SDL_KEYUP){
		reset_game_state(state, 1);
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

#endif /* GAME_C */
