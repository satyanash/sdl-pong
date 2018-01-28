
#ifndef UTIL_C
#define UTIL_C

// SDL includes
#include <SDL2/SDL.h>

void log_SDLError(FILE* outputfile, char* message){
	if(message != NULL){
		fprintf(outputfile, "ERROR: %s: ", message);
	}
	fprintf(outputfile, "error: %s\n", SDL_GetError());
}

#endif /* UTIL_C */
