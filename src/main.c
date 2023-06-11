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
#include "util.h"
#include "world.h"
#include "anim.h"
#include "linkedlist.h"
#include "storage.h"
#include "const.h"
#include "entity/entity.h"

static int update(void *userdata);
const char *fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont *font = NULL;
Entity *player;

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

		world = init_world(pd, "img/test_map.png");

		player = spawn_hook(pd, 50, -50);
		player->active = true;
		pd->sprite->setVisible(player->sprite, true);

		Entity* fish = spawn_fish(pd, 50, -200);
		fish->velocity->x = 100.0f;

		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
		pd->system->setUpdateCallback(update, pd);
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

	pd->sprite->drawSprites();
	pd->system->drawFPS(0, 0);

	float hook_x, hook_y;
	pd->sprite->getPosition(player->sprite, &hook_x, &hook_y);

	// Update all spawned bubbles
	// Update the position of the world background
	// and sprites that scroll with the map
	update_world(pd, dt, world, hook_x, hook_y);

	// pd->system->logToConsole("%f", world->world_pos->y);

	// Go through and update each entity that's in entity storage
	for (int i = 0; i < entity_storage->size; i++)
	{
		Entity *entity = linked_list->get(entity_storage, i);
		entity->update(pd, dt, world, entity);
	}

	return 1;
}
