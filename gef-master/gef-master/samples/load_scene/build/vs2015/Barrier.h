#pragma once
#ifndef  BARRIER_H
#define BARRIER_H

#include "Box2D\Box2D.h"
#include <set>
#include <vector>
#include <graphics/sprite.h>
#include <graphics/sprite_renderer.h>

#define BARWIDTH 20
#define BARHEIGHT 20

class barrier
{
public:
	barrier(float x, float y, b2World* world);
	barrier(float x, float y, b2World* world, char myWay);

	b2Body* body;
	gef::Sprite barrierSprite;
	char WaypointVal;

	void UpdateSprite();

	void draw(gef::SpriteRenderer* sprite_renderer);

};

#endif // ! BARRIER_H
