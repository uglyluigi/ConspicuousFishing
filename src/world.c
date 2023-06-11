#include "pd_api.h"
#include "util.h"
#include "vec.h"
#include "math.h"
#include "alloc.h"
#include "world.h"
#include "storage.h"

WorldInfo *init_world(PlaydateAPI *pd, const char *path)
{
	WorldInfo *world = MALLOC(1, WorldInfo);
	world->world_bg = alloc_bitmap(pd, path);
	world->scroll_acceleration = vec2d->new (0.0f, 0.0f);
	world->scroll_velocity = vec2d->new (0.0f, 0.0f);
	world->world_pos = vec2d->new (0.0f, 0.0f);

	int width, height;
	pd->graphics->getBitmapData(world->world_bg, &width, &height, NULL, NULL, NULL);
	world->world_dimensions = vec2d->new ((float)width, (float)height);

	return world;
}

void destroy_world(PlaydateAPI *pd, WorldInfo *world)
{
	free(world->scroll_acceleration);
	free(world->scroll_velocity);
	free(world->world_pos);
	pd->graphics->freeBitmap(world->world_bg);
	free(world->world_dimensions);
	free(world);
}

float get_scroll_acceleration(float hook_y)
{
	// Put the hook on a grid with 0 at the origin, 120 at the top and -120 at the bottom
	const float mapped_position = hook_y - (float)LCD_ROWS;

	// Negate the acceleration if we're at the top or bottom of the map
	// and the hook is in the upper half of the screen / lower half of the screen,
	// respectively
	if ((mapped_position > 0.0f && world->world_pos->y == 0.0f) ||
		(mapped_position < 0.0f && world->world_pos->y == world->world_dimensions->y - (float)LCD_ROWS))
	{
		return 0.0f;
	}

	return 8000.0f * -cosf(hook_y / 80);
}

void update_world(PlaydateAPI *pd, float dt, WorldInfo *world, float hook_x, float hook_y)
{
	// Apply the result of acceleration over this dt
	world->scroll_velocity->x += world->scroll_acceleration->x * dt;
	world->scroll_velocity->y += world->scroll_acceleration->y * dt;

	// compute the world offsets to render the background properly
	// and update all on-screen entities
	float x_offset = world->scroll_velocity->x * dt;
	float y_offset = world->scroll_velocity->y * dt;

	// actually scroll the background bitmap
	world->world_pos->x += x_offset;
	world->world_pos->y += y_offset;

	const min = -(world->world_dimensions->y - LCD_ROWS);
	const max = 0.0f;

	// Prevent things that scroll along with the map using this offset
	// from continuing to scroll when the bottom or top of the map was reached
	if (world->world_pos->y <= min || world->world_pos->y >= max)
	{
		y_offset = 0.0f;
	}

	// Prevent scrolling past the top and bottom of the world bitmap
	world->world_pos->y = clamp(world->world_pos->y, min, max);

	for (int i = 0; i < entity_storage->size; i++)
	{
		Entity *ent = linked_list->get(entity_storage, i);

		const float screen_top = 0.0f;
		const float screen_bottom = -(float)LCD_ROWS;

		// TODO maybe generalize this for things like map decorations
		// and other things that will scroll with the world and become
		// invisible/visible as they move off/on screen

		// pd->system->logToConsole("%f < %f < %f", screen_bottom, fish->world_pos->y, screen_top);

		// Set fish that are on-screen to visible and off-screen to invisible

		if (ent->entityType != Entity_Hook)
		{
			if (ent->world_pos->y < screen_top && ent->world_pos->y > screen_bottom)
			{
				ent->active = true;
				pd->sprite->setVisible(ent->sprite, true);
			}
			else
			{
				ent->active = false;
				pd->sprite->setVisible(ent->sprite, false);
			}
		}

		// Update the position of each fish
		// fish->world_pos->x += world->scroll_velocity->x * dt;
		ent->world_pos->y += -y_offset;
	}

	// Get the acceleration based on the hook's y-position
	float new_acceleration = get_scroll_acceleration(hook_y);

	// If the magnitude of the scroll acceleration is > 90,
	// cap it by negating the new acceleration.
	if (fabsf(new_acceleration) > 90.0f)
	{
		world->scroll_acceleration->y = -new_acceleration;
	}
	else // If the acceleration is <=90, stop moving the screen so it's not always moving
	{
		world->scroll_velocity->y = 0.0f;
		world->scroll_acceleration->y = 0.0f;
	}

	// Reset the scroll speed if the hook passes through the middle of the screen
	if (signf(world->scroll_acceleration->y) != signf(new_acceleration))
	{
		world->scroll_velocity->y = 0.0f;
	}

	pd->graphics->setDrawMode(kDrawModeNXOR);
	// Draw the updated word background bitmap
	pd->graphics->drawBitmap(world->world_bg, world->world_pos->x, world->world_pos->y, kBitmapUnflipped);
}
