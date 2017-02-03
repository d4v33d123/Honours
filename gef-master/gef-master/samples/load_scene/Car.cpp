#include "Car.h"

Car::Car(b2World* world)
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
	polygonShape.Set(vertices, 8);
	b2Fixture* fixture = body->CreateFixture(&polygonShape, 0.1f);//shape, density

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
	Tire* tire = new Tire(world);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, backTireMaxDriveForce, backTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(-3, 0.75); // -3, 0.75
	world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//back right tire
	tire = new Tire(world);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, backTireMaxDriveForce, backTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(3, 0.75); // 3, 0.75
	world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//front left tire
	tire = new Tire(world);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, frontTireMaxDriveForce, frontTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(-3, 8.75); // -3, 8.75
	flJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//front right tire
	tire = new Tire(world);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, frontTireMaxDriveForce, frontTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(3, 8.75); // 3, 8.75
	frJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);
	tires.push_back(tire);


	//set up sprite

	carBodySprite.set_width(5.5);
	carBodySprite.set_height(9);
	carBodySprite.set_colour(0xffffffff);
	carBodySprite.set_position(body->GetPosition().x, body->GetPosition().y + 45, 0.0f);
	carBodySprite.set_rotation(-body->GetAngle());

}


Car::~Car()
{

	for (int i = 0; i < tires.size(); i++)
		delete tires[i];
}

void Car::Update(int controlState) {
	for (int i = 0; i < tires.size(); i++)
		tires[i]->updateFriction();
	for (int i = 0; i < tires.size(); i++)
		tires[i]->updateDrive(controlState);

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
	

	UpdateSprites();
}

void Car::UpdateSprites()
{
	for (std::vector<Tire*>::size_type it = 0; it != tires.size(); it++)
	{
		tires[it]->UpdateSprite();
	}
	carBodySprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	carBodySprite.set_rotation(body->GetAngle());
}

void Car::draw(gef::SpriteRenderer* sprite_renderer)
{

	sprite_renderer->DrawSprite(carBodySprite);
	
	for (std::vector<Tire*>::size_type it = 0; it != tires.size(); it++)
	{
		tires[it]->draw(sprite_renderer);
	}

	

}

float Car::getXPosition()
{
	return body->GetPosition().x;
}

float Car::getYPosition()
{
	return body->GetPosition().y;
}