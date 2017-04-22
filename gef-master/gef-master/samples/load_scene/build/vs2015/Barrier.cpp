#include "Barrier.h"

// two barrier constructors depending if they have a waypoint or not attached to them
barrier::barrier(float x, float y, b2World* world, uint16 categoryBits, uint16 maskBits)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(BARWIDTH/2, BARHEIGHT/2);

	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 50;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	fixturedef.friction = 100;
	body->CreateFixture(&fixturedef);
	body->SetTransform(b2Vec2((x*BARWIDTH), (y*BARHEIGHT)), 0);

	mapx = x;
	mapy = y;

	body->SetUserData(this);

	barrierSprite.set_width(BARWIDTH);
	barrierSprite.set_height(BARHEIGHT);

	int colour = rand() % 99;
	UInt32 col = ((99 << 24 | (0 << 16) | (colour << 8) | (0 << 0))); 
	barrierSprite.set_colour(col);
	barrierSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	barrierSprite.set_rotation(-body->GetAngle());
}

barrier::barrier(float x, float y, b2World* world, uint16 categoryBits, uint16 maskBits, char myWay)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	body = world->CreateBody(&bodyDef);
	body->SetAngularDamping(5);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(BARWIDTH/2, BARHEIGHT/2);
	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 1;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	body->CreateFixture(&fixturedef);
	body->SetTransform(b2Vec2((x*BARWIDTH), (y*BARHEIGHT)), 0);

	mapx = x;
	mapy = y;

	body->SetUserData(this);

	barrierSprite.set_width(BARWIDTH);
	barrierSprite.set_height(BARHEIGHT); 
	int colour = rand() % 99;
	UInt32 col = ((99 << 24 | (0 << 16) | (colour << 8) | (0 << 0))); 
	barrierSprite.set_colour(col);
	barrierSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	barrierSprite.set_rotation(body->GetAngle());

	WaypointVal = myWay;
}


void barrier::UpdateSprite()
{
	barrierSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	barrierSprite.set_rotation(body->GetAngle());
}

void barrier::draw(gef::SpriteRenderer * sprite_renderer)
{
	sprite_renderer->DrawSprite(barrierSprite);
}
