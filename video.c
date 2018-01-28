

#ifndef VIDEO_C
#define VIDEO_C

// SDL includes
#include <SDL2/SDL.h>

#include "util.c"

struct VideoContext {
	SDL_Window* win;
	SDL_Renderer* ren;
};

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

int free_video_system(struct VideoContext* ctx){
	SDL_DestroyRenderer(ctx->ren);
	SDL_DestroyWindow(ctx->win);
	free(ctx);
	SDL_Quit();
}

#endif /* VIDEO_C */
