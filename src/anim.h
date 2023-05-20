#ifndef ANIMC
#define ANIMC

#include <stdbool.h>
#include "pd_api.h"

typedef struct BitmapTableAnimation
{
	// The index of current_image in the bitmap table
	int table_idx;
	// the accrued time since the last update
	float animation_timer;
	// duration of animation in seconds
	float time_between_frames;
	// the number of frames in the animation
	int num_frames;
	// the direction the animation is moving in
	// (1 = counting up in frames, -1 = counting down)
	int animation_direction;
	// does this animation loop?
	bool loops;
	// The current image that the user of this animation
	// should be showing
	LCDBitmap *current_image;
	// The actual bitmap table struct
	LCDBitmapTable *frames;
} BitmapTableAnimation;

typedef struct _animation_api
{
	BitmapTableAnimation *(*new_consecutive)(PlaydateAPI *, const char *, int, int, float, bool);
	void (*set_table_idx)(PlaydateAPI *, BitmapTableAnimation *, int);
	void (*advance_by)(PlaydateAPI *, BitmapTableAnimation *, float);
	void (*free)(PlaydateAPI *, BitmapTableAnimation *);
	void (*cleanup)(void);
} _animation_api;

_animation_api *animation;

BitmapTableAnimation *sta_new_empty();
BitmapTableAnimation *sta_new_consecutive(PlaydateAPI *pd, const char *path, int num_frames, int initial_sprite_idx, float time_between_frames, bool loops);
void sta_set_table_idx(PlaydateAPI *pd, BitmapTableAnimation *animation, int frame);
void sta_advance_by(PlaydateAPI *pd, BitmapTableAnimation *animation, float dt);
void cleanup_animation_api();
void init_animation_api();
void free_animation(PlaydateAPI *pd, BitmapTableAnimation *animation);

#endif