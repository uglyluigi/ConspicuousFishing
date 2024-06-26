#include <stdlib.h>
#include <math.h>

#include "vec.h"
#include "alloc.h"

// A 2-dimensional vector
// that supports basic vector operations like
// scaling, multiplying, adding, and copying

Vec2D *vec2d_new(float x, float y)
{
	Vec2D *vec2_ptr = MALLOC(1, Vec2D);
	vec2_ptr->x = x;
	vec2_ptr->y = y;
	return vec2_ptr;
}

void vec2d_add(const Vec2D *left, const Vec2D *right, Vec2D *out)
{
	out->x = left->x + right->x;
	out->y = left->y + left->y;
}

void vec2d_negate_x(const Vec2D *operand, Vec2D *out)
{
	out->x = -operand->x;
}

void vec2d_negate_y(const Vec2D *operand, Vec2D *out)
{
	out->y = -operand->y;
}

void vec2d_negate(const Vec2D *operand, Vec2D *out)
{
	vec2d_negate_x(operand, out);
	vec2d_negate_y(operand, out);
}

void vec2d_subtract(const Vec2D *left, const Vec2D *right, Vec2D *out)
{
	out->x = left->x - right->x;
	out->y = left->y - right->y;
}

void vec2d_scale(const Vec2D *operand, const float scale, Vec2D *out)
{
	out->x = scale * operand->x;
	out->y = scale * operand->y;
}

Vec2D *vec2d_copy(const Vec2D *src)
{
	Vec2D *vec2_ptr = MALLOC(1, Vec2D);
	vec2_ptr->x = src->x;
	vec2_ptr->y = src->y;
	return vec2_ptr;
}

float vec2d_magnitude(const Vec2D *u)
{
	return sqrt(u->x * u->x + u->y * u->y);
}

float vec2d_dot(const Vec2D *u, const Vec2D *v)
{
	return u->x * v->x + u->y * v->y;
}

float vec2d_theta(const Vec2D *u, const Vec2D *v)
{
	return acosf(vec2d_dot(u, v) / (vec2d_magnitude(u) * vec2d_magnitude(v)));
}

void cleanup_vec2d_api()
{
	free(vec2d);
}

void init_vec2d_api()
{
	vec2d = MALLOC(1, _vec2d_api);
	vec2d->add = &vec2d_add;
	vec2d->copy = &vec2d_copy;
	vec2d->negate = &vec2d_negate;
	vec2d->negate_x = &vec2d_negate_x;
	vec2d->negate_y = &vec2d_negate_y;
	vec2d->subtract = &vec2d_subtract;
	vec2d->scale = &vec2d_scale;
	vec2d->new = &vec2d_new;
	vec2d->magnitude = &vec2d_magnitude;
	vec2d->theta = &vec2d_theta;
	vec2d->dot = &vec2d_dot;
	vec2d->cleanup = &cleanup_vec2d_api;
}


