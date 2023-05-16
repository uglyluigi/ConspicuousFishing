#ifndef ANIMC
#define ANIMC

#include "pd_api.h"
#include "util.c"
#include "stdbool.h"

typedef struct {
	int table_idx;
	float animation_timer;
	// duration of animation in seconds
	float time_between_frames;
	int num_frames;
	int animation_direction;
	bool loops;
	LCDBitmap* current_image;
	LCDBitmapTable* frames;
} BitmapTableAnimation;

typedef struct {
	BitmapTableAnimation* (*new_consecutive)(PlaydateAPI*, const char*, int, int, float, bool);
	void (*set_table_idx)(PlaydateAPI*, BitmapTableAnimation*, int);
	void (*advance_by)(PlaydateAPI*, BitmapTableAnimation*, float);
} Animation;

Animation* animation;

BitmapTableAnimation* sta_new_empty() {
	BitmapTableAnimation* sta = (BitmapTableAnimation* ) malloc(sizeof(BitmapTableAnimation));
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
	BitmapTableAnimation* sta = sta_new_empty();
	LCDBitmapTable* frames = alloc_bitmap_table(pd, path);
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

void sta_advance_by(PlaydateAPI* pd, BitmapTableAnimation* animation, float dt) {
	animation->animation_timer += dt;

	if (animation->animation_timer > animation->time_between_frames) {
		if (animation->loops) {
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

void init_anim() {
	animation = (Animation*) malloc(sizeof(Animation));
	animation->advance_by = &sta_advance_by;
	animation->set_table_idx = &sta_set_table_idx;
	animation->new_consecutive = &sta_new_consecutive;
}

#endif