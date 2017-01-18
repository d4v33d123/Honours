#include "Barrier.h"

barrier::barrier(float width, float height, float x, float y, float angle, b2World* world)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(width, height);
	b2Fixture* fixture = body->CreateFixture(&polygonShape, 5);//shape, density
	body->SetTransform(b2Vec2(x, y), angle);

	body->SetUserData(this);

	barrierSprite.set_width(width);
	barrierSprite.set_height(height);
	barrierSprite.set_colour(0xffffffff);
	barrierSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	barrierSprite.set_rotation(-body->GetAngle());
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
