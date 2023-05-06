#ifndef VECC
#define VECC

#include <stdlib.h>

typedef struct Vec2D {
	float x;
	float y;
} Vec2D;

Vec2D* vec2d_new(float x, float y) {
	Vec2D* vec2_ptr = (Vec2D*) malloc(sizeof(Vec2D));
	vec2_ptr->x = x;
	vec2_ptr->y = y;
	return vec2_ptr;
}

void vec2d_add(const Vec2D* left, const Vec2D* right, Vec2D* out) {
	out->x = left->x + right->x;
	out->y = left->y + left->y;
}

void vec2d_negate_x(const Vec2D* operand, Vec2D* out) {
	out->x = -operand->x;
}

void vec2d_negate_y(const Vec2D* operand, Vec2D* out) {
	out->y = -operand->y;
}

void vec2d_negate(const Vec2D* operand, Vec2D* out) {
	vec2d_negate_x(operand, out);
	vec2d_negate_y(operand, out);
}

void vec2d_subtract(const Vec2D* left, const Vec2D* right, Vec2D* out) {
	out->x = left->x - right->x;
	out->y = left->y - right->y;
}

void vec2d_scale(const Vec2D* operand, const float scale, Vec2D* out) {
	out->x = scale * operand->x;
	out->y = scale * operand->y;
}

#endif



