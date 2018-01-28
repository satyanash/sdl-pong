#ifndef MUSIC_C
#define MUSIC_C

// SDL includes
#include <SDL2/SDL_mixer.h>

#define TOTAL_MUSICS 1

#define MUS_ONE 	0x01

struct MusicContext{
	Mix_Music* musics[TOTAL_MUSICS];
};

int MUSICS[] = {
	MUS_ONE,
};

char* get_music_filename(int code){
	switch(code){
		case MUS_ONE: return "./res/music.mp3";
		default: return NULL;
	}
}

struct MusicContext* load_musics(){
	struct MusicContext* ctx = malloc(sizeof (struct MusicContext));
	for(int i = 0; i < TOTAL_MUSICS; i++){
		ctx->musics[i] = Mix_LoadMUS(get_music_filename(MUSICS[i]));
	}
	return ctx;
}

void free_music_system(struct MusicContext* ctx){
	for(int i = 0; i < TOTAL_MUSICS; i++){
		Mix_FreeMusic(ctx->musics[i]);
	}
	free(ctx);
	//Quit SDL_mixer
	Mix_CloseAudio();
	Mix_Quit();
}


Mix_Music* get_music(const struct MusicContext* ctx, int code){
	for(int i = 0; i < TOTAL_MUSICS; i++){
		if(code == MUSICS[i]){
			return ctx->musics[i];
		}
	}
}

int start_music(const struct MusicContext* ctx, int code){
	//If there is no music playing 
	if( Mix_PlayingMusic() == 0) {
		//Play the music
		return Mix_PlayMusic( get_music(ctx, code), -1 );
	}
}

int stop_music(const struct MusicContext* ctx){
	return Mix_HaltMusic();
}

int toggle_music(const struct MusicContext* ctx, int code){
	//If there is no music playing 
	if( Mix_PlayingMusic() == 0) {
		//Play the music
		return Mix_PlayMusic( get_music(ctx, code), -1 );
	} else {
		return Mix_HaltMusic();
	}
}

#endif /* MUSIC_C */
