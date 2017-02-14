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
	

public:
	Car(b2World* world, uint16 categoryBits, uint16 maskBits, uint16 tirecategoryBits, uint16 tiremaskBits);
	Car() {};
	~Car();
	void Update(int controlState);
	void draw(gef::SpriteRenderer* sprite_renderer);
	float getXPosition();
	float getYPosition();
	gef::Sprite carBodySprite;
	void UpdateSprites();

	b2Body* body;
	std::vector<Tire*> tires;
	b2RevoluteJoint *flJoint, *frJoint;
};


#endif

