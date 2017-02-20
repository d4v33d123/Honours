#pragma once
#ifndef  BARRIER_H
#define BARRIER_H

#include "Box2D\Box2D.h"
#include <set>
#include <vector>
#include <graphics/sprite.h>
#include <graphics/sprite_renderer.h>

#define BARWIDTH 30
#define BARHEIGHT 30

class barrier
{
public:
	barrier(float x, float y, b2World* world, uint16 categoryBits, uint16 maskBits);
	barrier(float x, float y, b2World* world, uint16 categoryBits, uint16 maskBits, char myWay);

	b2Body* body;
	gef::Sprite barrierSprite;
	char WaypointVal;

	int mapx, mapy;

	void UpdateSprite();

	void draw(gef::SpriteRenderer* sprite_renderer);

};

#endif // ! BARRIER_H
