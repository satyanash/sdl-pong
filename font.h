#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL_ttf.h>

#define TOTAL_FONTS 2

#define FONT_OPEN_SANS 	0x01

struct FontContext{
	TTF_Font* fonts[TOTAL_FONTS];
};

int FONTS[][2] = {
	//Font, Size
	{FONT_OPEN_SANS, 16},
	{FONT_OPEN_SANS, 64}
};

#endif /* FONT_H */
