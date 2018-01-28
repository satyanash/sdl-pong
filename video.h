#ifndef VIDEO_H
#define VIDEO_H

#include "game.h"
#include "font.h"

struct VideoContext {
	SDL_Window* win;
	SDL_Renderer* ren;
};

#endif /* VIDEO_H */
