#include "Car.h"
#include "system\debug_log.h"

Car::Car(b2World* world, uint16 categoryBits, uint16 maskBits, uint16 tirecategoryBits, uint16 tiremaskBits, int numways, int laps)
{
	//create car body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	body = world->CreateBody(&bodyDef);
	body->SetAngularDamping(3);

	b2Vec2 vertices[8];
	vertices[0].Set(1.5, 0);
	vertices[1].Set(3, 2.5);
	vertices[2].Set(2.8, 5.5);
	vertices[3].Set(1, 10);
	vertices[4].Set(-1, 10);
	vertices[5].Set(-2.8, 5.5);
	vertices[6].Set(-3, 2.5);
	vertices[7].Set(-1.5, 0);
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(3, 5);

	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 0.1;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	body->CreateFixture(&fixturedef);


	body->SetTransform(b2Vec2((150), (150)), 0);

																	//prepare common joint parameters
	b2RevoluteJointDef jointDef;
	jointDef.bodyA = body;
	jointDef.enableLimit = true;
	jointDef.lowerAngle = 0;
	jointDef.upperAngle = 0;
	jointDef.localAnchorB.SetZero();//center of tire

	float maxForwardSpeed = 2500; //250
	float maxBackwardSpeed = -400; // -40
	float backTireMaxDriveForce = 3000; // 300
	float frontTireMaxDriveForce = 5000; // 500
	float backTireMaxLateralImpulse = 85; // 8.5
	float frontTireMaxLateralImpulse = 75; // 7.5

	//back left tire
	Tire* tire = new Tire(world, tirecategoryBits, tiremaskBits);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, backTireMaxDriveForce, backTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(-3, -5); // -3, 0.75
	world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//back right tire
	tire = new Tire(world, tirecategoryBits, tiremaskBits);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, backTireMaxDriveForce, backTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(3, -5); // 3, 0.75
	world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//front left tire
	tire = new Tire(world, tirecategoryBits, tiremaskBits);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, frontTireMaxDriveForce, frontTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(-3, 5); // -3, 8.75
	flJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//front right tire
	tire = new Tire(world, tirecategoryBits, tiremaskBits);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, frontTireMaxDriveForce, frontTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(3, 5); // 3, 8.75
	frJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);
	tires.push_back(tire);


	carBodySprite.set_width(6);
	carBodySprite.set_height(10);
	carBodySprite.set_colour(0xffffff00);
	carBodySprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	carBodySprite.set_rotation(-body->GetAngle());

	// set up our update buttons
	UpButton.set_width(2);
	UpButton.set_height(2);
	UpButton.set_colour(0xffffffff);
	UpButton.set_position(body->GetPosition().x, body->GetPosition().y - 2, 0.0f);
	UpButton.set_rotation(-body->GetAngle());

	LeftButton.set_width(2);
	LeftButton.set_height(2);
	LeftButton.set_colour(0xffffffff);
	LeftButton.set_position(body->GetPosition().x - 2, body->GetPosition().y, 0.0f);
	LeftButton.set_rotation(-body->GetAngle());

	RightButton.set_width(2);
	RightButton.set_height(2);
	RightButton.set_colour(0xffffffff);
	RightButton.set_position(body->GetPosition().x + 2, body->GetPosition().y, 0.0f);
	RightButton.set_rotation(-body->GetAngle());

	DownButton.set_width(2);
	DownButton.set_height(2);
	DownButton.set_colour(0xffffffff);
	DownButton.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	DownButton.set_rotation(-body->GetAngle());


	body->SetUserData(this);
	currentWaypoint = 0;
	currentlap = 0;
	laptime = new double[laps];
	MaxWays = numways;

}


Car::~Car()
{
	for (int i = 0; i < tires.size(); i++)
		delete tires[i];
	tires.clear();
}

void Car::Update(int controlState, std::vector<Waypoint*> wps) {
	for (int i = 0; i < tires.size(); i++)
		tires[i]->updateFriction();
	for (int i = 0; i < tires.size(); i++)
		tires[i]->updateDrive(controlState);

	savedcontrolstate = controlState;

	//control steering
	float lockAngle = 35 * DEGTORAD;
	float turnSpeedPerSec = 160 * DEGTORAD;//from lock to lock in 0.5 sec
	float turnPerTimeStep = turnSpeedPerSec / 60.0f;
	float desiredAngle = 0;
	switch (controlState & (TDC_LEFT | TDC_RIGHT)) 
	{
		case TDC_LEFT:  
			desiredAngle = -lockAngle;  
			break;
		case TDC_RIGHT: 
			desiredAngle = lockAngle; 
			break;
		default:;//nothing
	}
	float angleNow = flJoint->GetJointAngle();
	float angleToTurn = desiredAngle - angleNow;
	angleToTurn = b2Clamp(angleToTurn, -turnPerTimeStep, turnPerTimeStep);
	float newAngle = angleNow + angleToTurn;
	flJoint->SetLimits(newAngle, newAngle);
	frJoint->SetLimits(newAngle, newAngle);


	for (std::vector<Tire*>::size_type it = 0; it != tires.size(); it++)
	{
		tires[it]->updateFriction();
		tires[it]->updateDrive(controlState);
		tires[it]->updateTurn(controlState);
	}
	
	UpdateTime();
	UpdateSprites();

	//gef::DebugOut("WAYPOINT FOR CAR : %i", currentWaypoint);
}

void Car::UpdateSprites()
{
	for (std::vector<Tire*>::size_type it = 0; it != tires.size(); it++)
	{
		tires[it]->UpdateSprite();
	}
	carBodySprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	carBodySprite.set_rotation(body->GetAngle());
	
	DownButton.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	UpButton.set_position(body->GetPosition().x, body->GetPosition().y + 2, 0.0f);
	LeftButton.set_position(body->GetPosition().x - 2, body->GetPosition().y, 0.0f);
	RightButton.set_position(body->GetPosition().x + 2, body->GetPosition().y, 0.0f);
	
	
	updateButtonSprites();

}

void Car::draw(gef::SpriteRenderer* sprite_renderer)
{

	sprite_renderer->DrawSprite(carBodySprite);
	
	for (std::vector<Tire*>::size_type it = 0; it != tires.size(); it++)
	{
		tires[it]->draw(sprite_renderer);
	}

	sprite_renderer->DrawSprite(UpButton);
	sprite_renderer->DrawSprite(DownButton);
	sprite_renderer->DrawSprite(LeftButton);
	sprite_renderer->DrawSprite(RightButton);

}

float Car::getXPosition()
{
	return body->GetPosition().x;
}

float Car::getYPosition()
{
	return body->GetPosition().y;
}

void Car::UpdateWaypoint()
{
	gef::DebugOut("AYYY");
	if (currentWaypoint < MaxWays)
	{
		currentWaypoint += 1;
	}
	else
	{
		currentWaypoint = 0;
	}

	if (currentWaypoint == 1)
	{
		Lap();
	}
}

void Car::Lap()
{
	if (currentlap > 0)
	{
		finishlap = time(NULL);
		laptime[currentlap] = difftime(finishlap, startlap);
		
	}
	startlap = time(NULL);
	currentlap++;
}

void Car::UpdateTime()
{
	finishlap = time(NULL);
	currenttime = difftime(finishlap, startlap);
}

void Car::updateButtonSprites()
{
	float s = sinf(body->GetAngle());
	float c = cosf(body->GetAngle());

	gef::Vector2 pos = gef::Vector2(LeftButton.position().x(), LeftButton.position().y());
	pos.x -= carBodySprite.position().x();
	pos.y -= carBodySprite.position().y();

	float newx = (pos.x * c) - (pos.y * s);
	float newy = (pos.x * s) + (pos.y * c);
	
	gef::Vector2 newpos = gef::Vector2(carBodySprite.position().x(), carBodySprite.position().y());

	LeftButton.set_position(newpos.x + newx, newpos.y + newy, 0);
	LeftButton.set_rotation(body->GetAngle());

	pos = gef::Vector2(RightButton.position().x(), RightButton.position().y());
	pos.x -= carBodySprite.position().x();
	pos.y -= carBodySprite.position().y();

	newx = (pos.x * c) - (pos.y * s);
	newy = (pos.x * s) + (pos.y * c);

	newpos = gef::Vector2(carBodySprite.position().x(), carBodySprite.position().y());

	RightButton.set_position(newpos.x + newx, newpos.y + newy, 0);
	RightButton.set_rotation(body->GetAngle());

	pos = gef::Vector2(UpButton.position().x(), UpButton.position().y());
	pos.x -= carBodySprite.position().x();
	pos.y -= carBodySprite.position().y();

	newx = (pos.x * c) - (pos.y * s);
	newy = (pos.x * s) + (pos.y * c);

	newpos = gef::Vector2(carBodySprite.position().x(), carBodySprite.position().y());

	UpButton.set_position(newpos.x + newx, newpos.y + newy, 0);
	UpButton.set_rotation(body->GetAngle());

	DownButton.set_rotation(body->GetAngle());

	// update the sprite colours too 
	if (savedcontrolstate & (TDC_RIGHT))
	{
		LeftButton.set_colour(0xff0000ff);
	}
	else
	{
		LeftButton.set_colour(0xffffffff);
	}
	if (savedcontrolstate & (TDC_LEFT))
	{
		RightButton.set_colour(0xff0000ff);
	}
	else
	{
		RightButton.set_colour(0xffffffff);
	}
	if (savedcontrolstate & (TDC_UP))
	{
		UpButton.set_colour(0xff0000ff);
	}
	else
	{
		UpButton.set_colour(0xffffffff);
	}
	if (savedcontrolstate & (TDC_DOWN))
	{
		DownButton.set_colour(0xff0000ff);
	}
	else
	{
		DownButton.set_colour(0xffffffff);
	}

}