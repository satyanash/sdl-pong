
#ifndef FONT_C
#define FONT_C

#include <SDL2/SDL_ttf.h>

#include "font.h"
#include "video.h"
#include "util.c"

char* get_font_filename(int code){
	switch(code){
		case FONT_OPEN_SANS: return "./res/OpenSans-Regular.ttf";
		default: return NULL;
	}
}

int init_font_system(){
	// Initialize SDL TTF support
	if (TTF_Init() != 0){
		log_SDLError(stderr, "TTF_Init");
		SDL_Quit();
		return 1;
	}
}

struct FontContext* load_fonts(){
	struct FontContext* ctx = malloc(sizeof(struct FontContext));
	for(int i = 0; i < TOTAL_FONTS; i++){
		ctx->fonts[i] = TTF_OpenFont(get_font_filename(FONTS[i][0]), FONTS[i][1]);
		//Open the font
		if (ctx->fonts[i] == NULL){
			log_SDLError(stderr, "TTF_OpenFont: ");
		}
	}
	return ctx;
}

TTF_Font* get_font(const struct FontContext* ctx, int code, int size){
	for(int i = 0; i< TOTAL_FONTS; i++){
		if(code == FONTS[i][0] && size == FONTS[i][1]){
			return ctx->fonts[i];
		}
	}
}

SDL_Texture* render_text(struct VideoContext* video_ctx, struct FontContext* ctx, int font_code, int font_size, const char* message, SDL_Color color)
{
	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(get_font(ctx, font_code, font_size), message, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(video_ctx->ren, surf);
	SDL_FreeSurface(surf);
	return texture;
}

void free_font_system(struct FontContext* ctx){
	//Clean up the fonts
	for(int i = 0; i < TOTAL_FONTS; i++){
		TTF_CloseFont(ctx->fonts[i]);
	}
	free(ctx);
}

#endif /* FONT_C */
