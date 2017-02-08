#include "Barrier.h"

barrier::barrier(float x, float y, b2World* world)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(BARWIDTH, BARHEIGHT);
	b2Fixture* fixture = body->CreateFixture(&polygonShape, 5);//shape, density
	body->SetTransform(b2Vec2(x, y), 0);

	body->SetUserData(this);

	barrierSprite.set_width(BARWIDTH);
	barrierSprite.set_height(BARHEIGHT);
	barrierSprite.set_colour(0xffffffff);
	barrierSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	barrierSprite.set_rotation(-body->GetAngle());
}

barrier::barrier(float x, float y, b2World* world, char myWay)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(BARWIDTH, BARHEIGHT);
	b2Fixture* fixture = body->CreateFixture(&polygonShape, 5);//shape, density
	body->SetTransform(b2Vec2(x, y), 0);

	body->SetUserData(this);

	barrierSprite.set_width(BARWIDTH);
	barrierSprite.set_height(BARHEIGHT);
	barrierSprite.set_colour(0xffffffff);
	barrierSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	barrierSprite.set_rotation(-body->GetAngle());

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
