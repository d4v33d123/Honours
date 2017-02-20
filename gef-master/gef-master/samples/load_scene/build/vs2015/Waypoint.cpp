#include "Waypoint.h"
#include "system\debug_log.h"

Waypoint::Waypoint(float CAPx, float CAPy, float x, float y, int mapx, int mapy, b2World* world, uint16 categoryBits, uint16 maskBits, double** waypointOrder)
{
	float wayx, wayy;
	float width, height;
	float angle;

	// must do some calculations on our positions to get the angle and our waypoint position
	// we are going to use 1-tan of Opposite over adjacent
	for (int i = 0; i < 78; i++)
	{
		if ((int((waypointOrder[i][0]))) == (mapx) && (int((waypointOrder[i][1]))) == (mapy))
		{
			WaypointOrderVal = i;
		}
	}

	
	wayx = CAPx - x;
	wayx /= 2;
	wayx += x;

	wayy = CAPy - y;
	wayy /= 2;
	wayy += y;

	width = sqrt((pow((x - CAPx), 2) + (pow((y - CAPy), 2))));
	height = 0.1;

	angle = atan((y - CAPy) / (x - CAPx));



	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(width, height);

	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 5;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	body->CreateFixture(&fixturedef);
	body->SetTransform(b2Vec2(wayx, wayy), angle);

	body->SetUserData(this);

	barrierSprite.set_width(width);
	barrierSprite.set_height(height);
	barrierSprite.set_colour(0xff0000ff);
	barrierSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	barrierSprite.set_rotation(-body->GetAngle());
}


void Waypoint::UpdateSprite()
{
	barrierSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	barrierSprite.set_rotation(body->GetAngle());
}

void Waypoint::draw(gef::SpriteRenderer * sprite_renderer)
{
	sprite_renderer->DrawSprite(barrierSprite);
}
