#include "Barrier.h"

barrier::barrier(float x, float y, b2World* world, uint16 categoryBits, uint16 maskBits)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(BARWIDTH, BARHEIGHT);

	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 50;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	fixturedef.friction = 100;
	body->CreateFixture(&fixturedef);
	body->SetTransform(b2Vec2((x*BARWIDTH), (y*BARHEIGHT)), 0);

	body->SetUserData(this);

	barrierSprite.set_width(BARWIDTH);
	barrierSprite.set_height(BARHEIGHT);
	barrierSprite.set_colour(0xffffffff);
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
	polygonShape.SetAsBox(BARWIDTH, BARHEIGHT);
	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 1;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	body->CreateFixture(&fixturedef);
	body->SetTransform(b2Vec2((x*BARWIDTH), (y*BARHEIGHT)), 0);

	body->SetUserData(this);

	barrierSprite.set_width(BARWIDTH);
	barrierSprite.set_height(BARHEIGHT);
	barrierSprite.set_colour(0xffffffff);
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
