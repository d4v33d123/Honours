#include "AICar.h"

AICar::AICar(b2World* world, Net network, int ds, uint16 categoryBits, uint16 maskBits, uint16 tirecategoryBits, uint16 tiremaskBits)
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

	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 0.1;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	b2Fixture* fixture = body->CreateFixture(&fixturedef);//shape, density

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
	jointDef.localAnchorA.Set(-3, 0.75); // -3, 0.75
	world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//back right tire
	tire = new Tire(world, tirecategoryBits, tiremaskBits);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, backTireMaxDriveForce, backTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(3, 0.75); // 3, 0.75
	world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//front left tire
	tire = new Tire(world, tirecategoryBits, tiremaskBits);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, frontTireMaxDriveForce, frontTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(-3, 8.75); // -3, 8.75
	flJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);
	tires.push_back(tire);

	//front right tire
	tire = new Tire(world, tirecategoryBits, tiremaskBits);
	tire->setCharacteristics(maxForwardSpeed, maxBackwardSpeed, frontTireMaxDriveForce, frontTireMaxLateralImpulse);
	jointDef.bodyB = tire->body;
	jointDef.localAnchorA.Set(3, 8.75); // 3, 8.75
	frJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);
	tires.push_back(tire);


	//set up sprite

	carBodySprite.set_width(5.5);
	carBodySprite.set_height(9);
	carBodySprite.set_colour(0xffff00ff);
	carBodySprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	carBodySprite.set_rotation(-body->GetAngle());



	// set up our neural network depending on what input was recieved
	
	dataSize = ds;
	net_type = network;
	int nnl[] = { 4, 4, 10, 4 };
	switch (net_type)
	{
	case EBP:
		
		ebpNN = new BProp(4, nnl);
		break;
	case RPROP:
		
		rpNN = new RProp(4, nnl);
		break;
	case RMGSN:
		
		rmgsNN =  new RMGS(4, nnl);
		break;
	}
	current_control_states[0] = 0;
	current_control_states[1] = 0;
	current_control_states[2] = 0;
	current_control_states[3] = 0;


}

void AICar::Train(const char* fname)
{
	switch (net_type)
	{
	case EBP:
		ebpNN->Train(fname);
		break;
	case RPROP:
		rpNN->Train(fname, dataSize, 1);
		break;
	case RMGSN:
		rmgsNN->Train(fname, dataSize);
		break;
	}
}

void AICar::Update()
{
	
	UpdateNN(current_control_states);
	UpdateButtons();

	for (int i = 0; i < tires.size(); i++)
		tires[i]->updateFriction();
	for (int i = 0; i < tires.size(); i++)
		tires[i]->updateDrive(control_state);

	//control steering
	float lockAngle = 35 * DEGTORAD;
	float turnSpeedPerSec = 160 * DEGTORAD;//from lock to lock in 0.5 sec
	float turnPerTimeStep = turnSpeedPerSec / 60.0f;
	float desiredAngle = 0;
	switch (control_state & (TDC_LEFT | TDC_RIGHT))
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
		tires[it]->updateDrive(control_state);
		tires[it]->updateTurn(control_state);
	}


	UpdateSprites();

}

void AICar::UpdateNN(double* outputs)
{
	double inputsignal[4];
	switch (net_type)
	{
	case EBP:
		
		inputsignal[0] = angle_to_waypoint;
		inputsignal[1] = distance_to_side;
		inputsignal[2] = speed;
		inputsignal[3] = tire_angle;
		ebpNN->SetInputSignal(inputsignal);
		ebpNN->PropagateSignal();
		ebpNN->GetOutputSignal(outputs);
		break;

	case RPROP:
		
		inputsignal[0] = angle_to_waypoint;
		inputsignal[1] = distance_to_side;
		inputsignal[2] = speed;
		inputsignal[3] = tire_angle;
		rpNN->SetInputSignal(inputsignal);
		rpNN->PropagateSignal();
		rpNN->GetOutputSignal(outputs);

		break;

	case RMGSN:
		
		inputsignal[0] = angle_to_waypoint;
		inputsignal[1] = distance_to_side;
		inputsignal[2] = speed;
		inputsignal[3] = tire_angle;
		rmgsNN->SetInputSignal(inputsignal);
		rmgsNN->PropagateSignal();
		rmgsNN->GetOutputSignal(outputs);

		break;

	}



}

void AICar::UpdateButtons()
{

	if(current_control_states[0] >= 0.5 && prev_control_states[0] < 0.5)
		control_state |= TDC_LEFT;
	else if (current_control_states[0] < 0.5 && prev_control_states[0] >= 0.5)
		control_state &= ~TDC_LEFT;

	if (current_control_states[1] >= 0.5 && prev_control_states[1] < 0.5)
		control_state |= TDC_RIGHT;
	else if (current_control_states[1] < 0.5 && prev_control_states[1] >= 0.5)
		control_state &= ~TDC_RIGHT;

	if (current_control_states[2] >= 0.5 && prev_control_states[2] < 0.5)
		control_state |= TDC_UP;
	else if (current_control_states[2] < 0.5 && prev_control_states[2] >= 0.5)
		control_state &= ~TDC_UP;

	if (current_control_states[3] >= 0.5 && prev_control_states[3] < 0.5)
		control_state |= TDC_DOWN;
	else if (current_control_states[3] < 0.5 && prev_control_states[3] >= 0.5)
		control_state &= ~TDC_DOWN;


	for (int i = 0; i < 4; i++)
	{
		prev_control_states[i] = current_control_states[i];
		current_control_states[i] = 0;
	}
}
