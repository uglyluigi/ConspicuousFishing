#ifndef VECC
#define VECC

#include <stdlib.h>

// A 2-dimensional vector
// that supports basic vector operations like
// scaling, multiplying, adding, and copying
typedef struct Vec2D
{
	float x;
	float y;
} Vec2D;

typedef struct
{
	Vec2D *(*new)(float x, float y);
	Vec2D *(*copy)(const Vec2D *);
	void (*add)(const Vec2D *, const Vec2D *, Vec2D *);
	void (*negate_x)(const Vec2D *, Vec2D *);
	void (*negate_y)(const Vec2D *, Vec2D *);
	void (*negate)(const Vec2D *, Vec2D *);
	void (*subtract)(const Vec2D *, const Vec2D *, Vec2D *);
	void (*scale)(const Vec2D *, const float, Vec2D *);
	float (*magnitude)(const Vec2D *);
	float (*dot)(const Vec2D *, const Vec2D *);
	float (*theta)(const Vec2D *, const Vec2D *);
} _vec2d_api;

_vec2d_api *vec2d;

Vec2D *vec2d_new(float x, float y)
{
	Vec2D *vec2_ptr = (Vec2D *)malloc(sizeof(Vec2D));
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
	Vec2D *vec2_ptr = (Vec2D *)malloc(sizeof(Vec2D));
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

void init_vec2d_api()
{
	vec2d = (_vec2d_api *)malloc(sizeof(_vec2d_api));
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
}
#endif
