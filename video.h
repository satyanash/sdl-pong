#ifndef VIDEO_H
#define VIDEO_H

#include "game.h"
#include "font.h"

struct VideoContext {
	SDL_Window* win;
	SDL_Renderer* ren;
};

int init_video_system();

struct VideoContext* load_video(char* title, int x, int y, int height, int width);

void render_game( const struct GameState* state, struct VideoContext* video_ctx, struct FontContext* font_ctx, SDL_Rect* screen_rect);

int free_video_system(struct VideoContext* ctx);

#endif /* VIDEO_H */
