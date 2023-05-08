//
//  main.c
//  Extension
//
//  Created by Dave Hayden on 7/30/14.
//  Copyright (c) 2014 Panic, Inc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "fish.c"
#include "bubble.c"
#include "util.c"

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

LCDBitmap* background_bitmap = NULL;
LCDBitmap* hook_bitmap = NULL; 

FishEntity* fishes[MAX_FISH];

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed
	
	if ( event == kEventInit )
	{
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		
		if ( font == NULL )
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);
			
		pd->system->resetElapsedTime();

		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
		pd->system->setUpdateCallback(update, pd);
		
		for (int i = 0; i < MAX_FISH; i++) {
			fishes[i] = alloc_fish(pd, get_sprite_for_fish_type(Goldfish));
		}
	}
	
	return 0;
}

static int update(void* userdata)
{
	PlaydateAPI* pd = userdata;
	
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	//pd->graphics->drawText("Hello World!", strlen("Hello World!"), kASCIIEncoding, x, y);

	pd->sprite->drawSprites();
	pd->system->drawFPS(0,0);
	
	fish_tick(pd, fishes, MAX_FISH);
	do_bubble_ticks(pd);

	return 1;
}



