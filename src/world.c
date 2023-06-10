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
	const float mapped_position = hook_y - (float)LCD_ROWS;

	if ((mapped_position > 0.0f && world->world_pos->y == 0.0f) ||
		(mapped_position < 0.0f && world->world_pos->y == world->world_dimensions->y - (float)LCD_ROWS))
	{
		return 0.0f;
	}

	return 8000.0f * -cosf(hook_y / 80);
}

void update_world(PlaydateAPI *pd, float dt, WorldInfo *world, float hook_x, float hook_y)
{
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
	// Prevent scrolling past the top and bottom of the world bitmap

	if (world->world_pos->y <= min || world->world_pos->y >= max)
	{
		y_offset = 0.0f;
	}

	world->world_pos->y = clamp(world->world_pos->y, min, max);

	for (int i = 0; i < entity_storage->size; i++)
	{
		EntityPointer *entity_ptr_info = linked_list->get(entity_storage, i);

		switch (entity_ptr_info->kind)
		{
		case kFishEntity:
		{
			FishEntity *fish = entity_ptr_info->data.fish;

			const float screen_top = 0.0f;
			const float screen_bottom = -(float)LCD_ROWS;

			// TODO maybe generalize this for things like map decorations
			// and other things that will scroll with the world and become
			// invisible/visible as they move off/on screen

			// pd->system->logToConsole("%f < %f < %f", screen_bottom, fish->world_pos->y, screen_top);

			if (fish->world_pos->y < screen_top && fish->world_pos->y > screen_bottom)
			{
				pd->sprite->setVisible(fish->sprite, true);
			} else {
				pd->sprite->setVisible(fish->sprite, false);
			}

			// Update the position of each fish 
			// fish->world_pos->x += world->scroll_velocity->x * dt;
			fish->world_pos->y += -y_offset;

			pd->system->logToConsole("Scroll accel: (%f, %f) vel: (%f, %f)", world->scroll_acceleration->x, world->scroll_acceleration->y, world->scroll_velocity->x, world->scroll_velocity->y);
			pd->system->logToConsole("Yworld=%f Yfish=%f", world->world_pos->y, fish->world_pos->y);
			break;
		}
		default:
			break;
		}
	}

	float new_acceleration = get_scroll_acceleration(hook_y);

	if (fabsf(new_acceleration) > 80.0f)
	{
		world->scroll_acceleration->y = -new_acceleration;
	}
	else
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
	pd->graphics->drawBitmap(world->world_bg, world->world_pos->x, world->world_pos->y, kBitmapUnflipped);
}
