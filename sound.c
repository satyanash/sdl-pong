#ifndef SOUND_C
#define SOUND_C

// SDL includes
#include <SDL2/SDL_mixer.h>

#define TOTAL_SOUNDS 3

#define MIX_DEAD 	0x01
#define MIX_KNOCK 	0x02
#define MIX_BOUNCE 	0x03

struct SoundContext{
	Mix_Chunk* sounds[TOTAL_SOUNDS];
};

int SOUNDS[] = {
	MIX_DEAD,
	MIX_KNOCK,
	MIX_BOUNCE,
};

char* get_sound_filename(int code){
	switch(code){
		case MIX_DEAD: return "./res/dead.wav";
		case MIX_KNOCK: return "./res/knock.wav";
		case MIX_BOUNCE: return "./res/bounce.wav";
		default: return NULL;
	}
}

int init_audio_system(){
	// Initialize the SDL sound system
	return Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 1, 4096 );
}

void free_sound_system(struct SoundContext* ctx){
	for(int i = 0; i < TOTAL_SOUNDS; i++){
		Mix_FreeChunk(ctx->sounds[i]);
	}
	free(ctx);
	//Quit SDL_mixer
	Mix_CloseAudio();
	Mix_Quit();
}

struct SoundContext* load_sounds(){
	struct SoundContext* ctx = malloc(sizeof (struct SoundContext));
	for(int i = 0; i < TOTAL_SOUNDS; i++){
		ctx->sounds[i] = Mix_LoadWAV(get_sound_filename(SOUNDS[i]));
	}
	return ctx;
}

Mix_Chunk* get_chunk(const struct SoundContext* ctx, int code){
	for(int i = 0; i< TOTAL_SOUNDS; i++){
		if(code == SOUNDS[i]){
			return ctx->sounds[i];
		}
	}
}

void play_sound(const struct SoundContext* ctx, int code){
	Mix_PlayChannel(-1, get_chunk(ctx, code), 0);
}

#endif /* SOUND_C */
