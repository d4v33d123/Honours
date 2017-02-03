#ifndef CAR_H
#define CAR_H

#include <Box2D/Box2D.h>
#include <graphics/sprite.h>
#include "Tire.h"
#include <graphics/sprite_renderer.h>


#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#endif

class Car
{
protected:
	b2Body* body;
	std::vector<Tire*> tires;
	b2RevoluteJoint *flJoint, *frJoint;
	gef::Sprite carBodySprite;
	void UpdateSprites();

public:
	Car(b2World* world);
	Car() {};
	~Car();
	void Update(int controlState);
	void draw(gef::SpriteRenderer* sprite_renderer);
	float getXPosition();
	float getYPosition();
};


#endif

