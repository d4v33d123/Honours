#pragma once
#ifndef  BARRIER_H
#define BARRIER_H

#include "Box2D\Box2D.h"
#include <set>
#include <vector>
#include <graphics/sprite.h>
#include <graphics/sprite_renderer.h>

class barrier
{
	barrier(float width, float height, float x, float y, float angle, b2World* world);

	b2Body* body;
	gef::Sprite barrierSprite;

	void UpdateSprite();

	void draw(gef::SpriteRenderer* sprite_renderer);

};

#endif // ! BARRIER_H
