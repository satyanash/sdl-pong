// SDL includes
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

// our headers
#include "game.h"
#include "game.c"

// custom includes
#include "util.c"

#include "video.h"
#include "video.c"

#include "font.h"
#include "font.c"

#include "sound.c"
#include "music.c"

SDL_Rect screen_rect = {
	.x = 0,
	.y = 0,
	.w = SCREEN_WIDTH,
	.h = SCREEN_HEIGHT,
};

int main(const int argc, const char **argv)
{
	// init video
	init_video_system();
	struct VideoContext* video_ctx = load_video("Breakout Pong", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// init fonts
	init_font_system();
	struct FontContext* font_ctx = load_fonts();

	// init audio
	init_audio_system();
	struct SoundContext* sound_ctx = load_sounds();
	struct MusicContext* music_ctx = load_musics();

	// Initialize SDL Image
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
		log_SDLError(stderr, "IMG_Init");
		SDL_Quit();
		return 1;
	}


	struct GameState state;
	reset_game_state(&state, 1);

	SDL_Event e;
	while (!state.quit){
		/**
		 * General event loop is as follows:
		 * 1. Events
		 * 2. Logic
		 * 3. Rendering
		 */

		// 1. handle all Events in the queue, mutate state accordingly 
		while (SDL_PollEvent(&e)){
			handle_event(&e, &state);
		}

		// 2. do whatever logic needs to be done
		perform_game_logic(&state, sound_ctx, music_ctx);

		// 3. Render the scene
		render_game(&state, video_ctx, font_ctx, &screen_rect);
	}

	free_music_system(music_ctx);
	free_sound_system(sound_ctx);
	free_video_system(video_ctx);

	IMG_Quit();
	return 0;
}
