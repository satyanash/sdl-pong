
#ifndef UTIL_C
#define UTIL_C

#include <stdbool.h>

// SDL includes
#include <SDL2/SDL.h>

void log_SDLError(FILE* outputfile, char* message){
	if(message != NULL){
		fprintf(outputfile, "ERROR: %s: ", message);
	}
	fprintf(outputfile, "error: %s\n", SDL_GetError());
}


bool intersects( SDL_Rect* A, SDL_Rect* B){
	if((A->x + A->w) < B->x || (B->x + B->w) < A->x){
		return false;
	}
	if((A->y + A->h) < B->y || (B->y + B->h) < A->y){
		return false;
	}
	return true;
}

#endif /* UTIL_C */
