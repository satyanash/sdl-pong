#ifndef VIDEO_C
#define VIDEO_C

// SDL includes
#include <SDL2/SDL.h>

// our headers
#include "game.h"
#include "video.h"

#include "util.c"
#include "font.c"

int init_video_system(){
	// Initialize SDL Video
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		log_SDLError(stderr, "SDL_Init");
		return 1;
	} 
}

struct VideoContext* load_video(char* title, int x, int y, int height, int width){
	struct VideoContext* ctx = malloc(sizeof(struct VideoContext));
	ctx->win = SDL_CreateWindow(title, x, y, height, width, SDL_WINDOW_SHOWN);
	ctx->ren = SDL_CreateRenderer(ctx->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	return ctx;
}

void render_game( const struct GameState* state, struct VideoContext* video_ctx, struct FontContext* font_ctx, SDL_Rect* screen_rect){

	//SDL_SetRenderDrawColor(ren, 97, 132, 247, SDL_ALPHA_OPAQUE);
	SDL_SetRenderDrawColor(video_ctx->ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(video_ctx->ren, screen_rect);

	SDL_SetRenderDrawColor(video_ctx->ren, 255, 255, 255, SDL_ALPHA_OPAQUE);

	if(state->paddle.exists){
		// draw the paddle
		SDL_Rect paddle;
		paddle.x = state->paddle.position.x;
		paddle.y = state->paddle.position.y;
		paddle.w = state->paddle.width;
		paddle.h = state->paddle.height;

		SDL_RenderFillRect(video_ctx->ren, &paddle);
	}

	// draw the ball
	SDL_Rect ball;
	ball.x = state->ball.position.x;
	ball.y = state->ball.position.y;
	ball.w = state->ball.width;
	ball.h = state->ball.height;

	//draw the tiles
	for(int i = 0; i < MAX_TILE_ROWS; i++){
		for(int j = 0; j < MAX_TILE_COLS; j++){
			struct Tile* tile = state->tile_map->tiles[i][j];
			if( ! tile->broken){
				SDL_Rect tile_rect;
				tile_rect.x = j * MIN_TILE_WIDTH;
				tile_rect.y = i * MIN_TILE_HEIGHT;
				tile_rect.w = MIN_TILE_WIDTH;
				tile_rect.h = MIN_TILE_HEIGHT;

				SDL_RenderFillRect(video_ctx->ren, &tile_rect);
			}
		}
	}

	SDL_RenderFillRect(video_ctx->ren, &ball);
	if(state->gameover){
		SDL_Color color = { 255, 255, 255, 255 };
		SDL_Texture* text = render_text(video_ctx, font_ctx, FONT_OPEN_SANS, 64, "GAME OVER!", color);

		SDL_Rect dst;
		SDL_QueryTexture(text, NULL, NULL, &dst.w, &dst.h); //Query the texture to get its width and height to use
		dst.x = (screen_rect->w / 2) - dst.w / 2;
		dst.y = (screen_rect->h / 2) - dst.h / 2;

		SDL_RenderCopy(video_ctx->ren, text, NULL, &dst);

		SDL_Texture* subtext = render_text(video_ctx, font_ctx, FONT_OPEN_SANS, 16, "press ENTER to play again, Q to quit", color);
		SDL_Rect subdst;
		SDL_QueryTexture(subtext, NULL, NULL, &subdst.w, &subdst.h);
		subdst.x = (screen_rect->w / 2) - subdst.w / 2;
		subdst.y = (screen_rect->h / 2) + dst.h / 2 + 20;

		SDL_RenderCopy(video_ctx->ren, subtext, NULL, &subdst);
	}

	if(state->paused){
		SDL_Color color = { 255, 255, 255, 255 };
		SDL_Texture* text = render_text(video_ctx, font_ctx, FONT_OPEN_SANS, 64, "PAUSED", color);

		SDL_Rect dst;
		SDL_QueryTexture(text, NULL, NULL, &dst.w, &dst.h); //Query the texture to get its width and height to use
		dst.x = (screen_rect->w / 2) - dst.w / 2;
		dst.y = (screen_rect->h / 2) - dst.h / 2;

		SDL_RenderCopy(video_ctx->ren, text, NULL, &dst);
	}


	// finally present the rendered stuff
	SDL_RenderPresent(video_ctx->ren);
}

int free_video_system(struct VideoContext* ctx){
	SDL_DestroyRenderer(ctx->ren);
	SDL_DestroyWindow(ctx->win);
	free(ctx);
	SDL_Quit();
}

#endif /* VIDEO_C */
