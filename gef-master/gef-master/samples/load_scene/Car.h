#ifndef CAR_H
#define CAR_H

#include <Box2D/Box2D.h>
#include <graphics/sprite.h>
#include "Tire.h"
#include "build\vs2015\Waypoint.h"
#include <graphics/sprite_renderer.h>
#include <time.h>
#include "assets\png_loader.h"


#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#endif

class Car
{
protected:
	

public:
	Car(b2World* world, uint16 categoryBits, uint16 maskBits, uint16 tirecategoryBits, uint16 tiremaskBits, int numways, int laps);
	Car() {};
	~Car();
	void Update(int controlState, std::vector<Waypoint*> wps);
	void draw(gef::SpriteRenderer* sprite_renderer);
	float getXPosition();
	float getYPosition();
	void UpdateSprites();
	void Lap();
	void UpdateTime();
	void UpdateWaypoint();

	b2Body* body;
	std::vector<Tire*> tires;
	b2RevoluteJoint *flJoint, *frJoint;
	int currentWaypoint;
	int MaxWays;

	gef::Sprite carBodySprite;

	// lap realted variables
	int currentlap;
	double* laptime;
	double currenttime;

	time_t startlap;
	time_t finishlap;

	// button presses
	gef::Sprite UpButton;
	gef::Sprite DownButton;
	gef::Sprite LeftButton;
	gef::Sprite RightButton;

	// function to update button sprites
	void updateButtonSprites();
	int savedcontrolstate; //for updating the colour of the buttons

};


#endif

