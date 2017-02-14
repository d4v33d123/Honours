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
public:
	Waypoint(float CAPx, float CAPy, float x, float y, b2World* world, uint16 categoryBits, uint16 maskBits, double** waypointOrder);

	b2Body* body;
	gef::Sprite barrierSprite;

	void UpdateSprite();

	void draw(gef::SpriteRenderer* sprite_renderer);

	bool Passed;

	int WaypointOrderVal;

};
#endif // !WAYPOINT_H
