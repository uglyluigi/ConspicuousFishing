#ifndef VECC
#define VECC

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
	void (*cleanup)(void);
} _vec2d_api;

_vec2d_api *vec2d;

Vec2D *vec2d_new(float x, float y);
void vec2d_add(const Vec2D *left, const Vec2D *right, Vec2D *out);
void vec2d_negate_x(const Vec2D *operand, Vec2D *out);
void vec2d_negate_y(const Vec2D *operand, Vec2D *out);
void vec2d_negate(const Vec2D *operand, Vec2D *out);
void vec2d_subtract(const Vec2D *left, const Vec2D *right, Vec2D *out);
void vec2d_scale(const Vec2D *operand, const float scale, Vec2D *out);
Vec2D *vec2d_copy(const Vec2D *src);
float vec2d_magnitude(const Vec2D *u);
float vec2d_dot(const Vec2D *u, const Vec2D *v);
float vec2d_theta(const Vec2D *u, const Vec2D *v);
void cleanup_vec2d_api();
void init_vec2d_api();

#endif