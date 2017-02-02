#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "Box2D\Box2D.h"
#include <math.h>
#include <set>
#include <vector>
#include <graphics/sprite.h>
#include <graphics/sprite_renderer.h>

class Waypoint
{
	Waypoint(float CAPx, float CAPy, float x, float y, b2World* world, int number);

	b2Body* body;
	gef::Sprite barrierSprite;

	void UpdateSprite();

	void draw(gef::SpriteRenderer* sprite_renderer);

	int WaypointNumber;

	bool Passed;



};
#endif // !WAYPOINT_H
