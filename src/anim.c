#include <stdbool.h>

#include "pd_api.h"
#include "util.h"
#include "anim.h"
#include "alloc.h"

// Private function that makes a clean BitmapTableAnimation struct
BitmapTableAnimation *sta_new_empty()
{
	BitmapTableAnimation *sta = MALLOC(1, BitmapTableAnimation);
	sta->table_idx = 0;
	sta->animation_timer = 0.0f;
	sta->animation_direction = 1;
	sta->time_between_frames = 0.0f;
	sta->current_image = NULL;
	sta->num_frames = 0;
	sta->loops = false;
	sta->frames = NULL;
	return sta;
}

BitmapTableAnimation *sta_new_consecutive(PlaydateAPI *pd, const char *path, int num_frames, int initial_sprite_idx, float time_between_frames, bool loops)
{
	BitmapTableAnimation *sta = sta_new_empty();
	LCDBitmapTable *frames = alloc_bitmap_table(pd, path);
	sta->frames = frames;
	sta->current_image = pd->graphics->getTableBitmap(frames, initial_sprite_idx);
	sta->num_frames = num_frames;
	sta->time_between_frames = time_between_frames;
	sta->loops = loops;
	return sta;
}

void sta_set_table_idx(PlaydateAPI *pd, BitmapTableAnimation *animation, int frame)
{
	animation->table_idx = frame;
	animation->current_image = pd->graphics->getTableBitmap(animation->frames, frame);
}

void sta_advance_by(PlaydateAPI *pd, BitmapTableAnimation *animation, float dt)
{
	animation->animation_timer += dt;

	if (animation->animation_timer > animation->time_between_frames)
	{
		if (animation->loops)
		{
			if (animation->table_idx == animation->num_frames)
			{
				animation->animation_direction = -1;
			}
			else if (animation->table_idx == 0)
			{
				animation->animation_direction = 1;
			}
		}

		animation->table_idx += animation->animation_direction;
		sta_set_table_idx(pd, animation, animation->table_idx);
		animation->animation_timer = 0.0f;
	}
}

void cleanup_animation_api()
{
	free(animation);
}

void init_animation_api()
{
	animation = MALLOC(1, _animation_api);
	animation->advance_by = &sta_advance_by;
	animation->set_table_idx = &sta_set_table_idx;
	animation->new_consecutive = &sta_new_consecutive;
	animation->cleanup = &cleanup_animation_api;
}

void free_animation(PlaydateAPI *pd, BitmapTableAnimation *animation)
{
	// Doing this would probably be a double free
	// since current_image is a pointer into the bitmap table
	// which is deallocated in the next line
	// pd->graphics->freeBitmap(animation->current_image);
	pd->graphics->freeBitmapTable(animation->frames);
	free(animation);
}