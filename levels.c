#ifndef LEVELS_C
#define LEVELS_C

// our headers
#include "game.h"

struct TileMap* get_level(int level){
	if(level == 1){
		struct TileMap* map = malloc(sizeof(struct TileMap));
		for(int i = 0; i < MAX_TILE_ROWS; i++){
			for(int j = 0; j < MAX_TILE_COLS; j++){
				map->tiles[i][j] = malloc(sizeof(struct Tile));
				//map->tiles[i][j]->position.x = i * MIN_TILE_WIDTH;
				//map->tiles[i][j]->position.y = j * MIN_TILE_HEIGHT;
				//map->tiles[i][j]->width = MIN_TILE_WIDTH;
				//map->tiles[i][j]->height = MIN_TILE_HEIGHT;
				map->tiles[i][j]->broken = false;
			}
		}
		return map;
	}
	else return NULL;
}

#endif /* LEVELS_C */
