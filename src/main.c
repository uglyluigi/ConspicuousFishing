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
#include "fish.h"
#include "bubble.h"
#include "util.h"
#include "fish_hook.h"
#include "world.h"
#include "anim.h"
#include "linkedlist.h"
#include "storage.h"
#include "const.h"

static int update(void *userdata);
const char *fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont *font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif
	int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if (event == kEventInit)
	{
		init_animation_api();
		init_util_api();
		init_vec2d_api();
		init_linkedlist_api();
		init_storage();

		const char *err;
		font = pd->graphics->loadFont(fontpath, &err);

		if (font == NULL)
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		pd->system->resetElapsedTime();

		init_hook(pd);
		world = init_world(pd, "img/test_map.png");

		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
		pd->system->setUpdateCallback(update, pd);

		alloc_fish(pd, get_sprite_for_fish_type(Goldfish));
	}

	if (event == kEventTerminate)
	{
		util->cleanup();
		animation->cleanup();
		vec2d->cleanup();
		clean_storage();
	}

	return 0;
}

static float prev_time = 0;

// delta time (in s)
static float dt = 0;

static int update(void *userdata)
{
	PlaydateAPI *pd = userdata;

	// setup delta time
	dt = 0.001f * (pd->system->getCurrentTimeMilliseconds() - prev_time);
	prev_time = pd->system->getCurrentTimeMilliseconds();

	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	// pd->graphics->drawText("Hello World!", strlen("Hello World!"), kASCIIEncoding, x, y);

	pd->sprite->drawSprites();
	pd->system->drawFPS(0, 0);

	float hook_x, hook_y;
	pd->sprite->getPosition(player->sprite, &hook_x, &hook_y);

	fish_tick(pd, dt, fishes, MAX_FISH);
	do_bubble_ticks(pd, dt);
	do_fish_hook_ticks(pd, dt);
	update_world(pd, dt, world, hook_x, hook_y);

	return 1;
}
