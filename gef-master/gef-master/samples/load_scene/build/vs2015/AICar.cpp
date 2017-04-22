#include "AICar.h"
#include "system\debug_log.h"

AICar::AICar(b2World* world, Net network, int ds, uint16 categoryBits, uint16 maskBits, uint16 tirecategoryBits, uint16 tiremaskBits, int numways, int laps)
{
	//create car body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	body = world->CreateBody(&bodyDef);
	body->SetAngularDamping(3);

	//body->SetTransform(b2Vec2(200, 200), 0);

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
	//polygonShape.Set(vertices, 8);
	polygonShape.SetAsBox(3, 5);

	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 0.1;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	b2Fixture* fixture = body->CreateFixture(&fixturedef);//shape, density

	//body->SetTransform(b2Vec2((150), (150)), 0);

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


	
	
	
	//set up sprite

	carBodySprite.set_width(6);
	carBodySprite.set_height(10);
	carBodySprite.set_colour(0xffffffff);
	carBodySprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	carBodySprite.set_rotation(-body->GetAngle());

	LeftSprite.set_width(50);
	LeftSprite.set_height(0.3);
	LeftSprite.set_colour(0xff0000ff);
	LeftSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	LeftSprite.set_rotation(-body->GetAngle());

	LeftHitSprite.set_width(10);
	LeftHitSprite.set_height(10);
	LeftHitSprite.set_colour(0xff0000ff);
	LeftHitSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	LeftHitSprite.set_rotation(-body->GetAngle());

	RightHitSprite.set_width(10);
	RightHitSprite.set_height(10);
	RightHitSprite.set_colour(0xff0000ff);
	RightHitSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	RightHitSprite.set_rotation(-body->GetAngle());

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


	// set up our neural network depending on what input was recieved
	
	dataSize = ds;
	net_type = network;

	int ennl[] = { 4, 50,  4 }; //{ 4, 4, 4,  4 }; dat 25 = err < 0.20
	int rpnnl[] = { 4, 100, 4 }; // 4, 100, 4 dat 25 = err < 0.06 after 5k iterations
	int rmnnl[] = { 4, 4, 50, 4 };


	switch (net_type)
	{
	case EBP:
		
		ebpNN = new BProp(3, ennl);
		break;
	case RPROP:
		
		rpNN = new RProp(3, rpnnl);
		break;
	case RMGSN:
		
		rmgsNN =  new RMGS(4, rmnnl);
		break;
	}
	current_control_states[0] = 0;
	current_control_states[1] = 0;
	current_control_states[2] = 0;
	current_control_states[3] = 0;

	prev_control_states[0] = 0;
	prev_control_states[1] = 0;
	prev_control_states[2] = 0;
	prev_control_states[3] = 0;

	tire_angle = 0.5;


	body->SetUserData(this);

	currentWaypoint = 0;
	control_state = 0;
	currentlap = 0;
	laptime = new double[laps];
	MaxWays = numways;
}

AICar::~AICar()
{
	for (int i = 0; i < tires.size(); i++)
		delete tires[i];

	tires.clear();

	switch (net_type)
	{
	case EBP:
		//ebpNN->~BProp();
		delete ebpNN;
		ebpNN = NULL;
		break;
	case RPROP:
		//rpNN->~RProp();
		delete rpNN;
		rpNN = NULL;
		break;
	case RMGSN:
		//rmgsNN->~RMGS();
		delete rmgsNN;
		rmgsNN = NULL;
		break;
	}


}

void AICar::Train(const char* fname)
{
	switch (net_type)
	{
	case EBP:
		ebpNN->Run(fname, dataSize, 10000);
		gef::DebugOut("trained ebp");
		break;
	case RPROP:
		rpNN->Train(fname, dataSize, 4);
		rpNN->Accuracy(fname, dataSize);
		gef::DebugOut("trained RPROP");
		break;
	case RMGSN:
		rmgsNN->Train(fname, dataSize);
		gef::DebugOut("trained RMGS");
		break;
	}
}

void AICar::Update(std::vector<Waypoint*> wps, std::vector<barrier*> bars, b2World* world)
{

	UpdateRaycasts(bars, world);

	UpdateNN(wps);

	float newAngle = 0;		

	UpdateButtons();

	//gef::DebugOut("CONTROL STATES: %i", control_state);

	for (int i = 0; i < tires.size(); i++)
		tires[i]->updateFriction();
	for (int i = 0; i < tires.size(); i++)
		tires[i]->updateDrive(control_state);

	//control steering
	float lockAngle = 35 * DEGTORAD;
	float turnSpeedPerSec = 160 * DEGTORAD;//from lock to lock in 0.5 sec
	float turnPerTimeStep = turnSpeedPerSec / 60.0f;
	float desiredAngle = 0;
	//for the button press sprites
	savedcontrolstate = control_state;

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
	newAngle = angleNow + angleToTurn;
	flJoint->SetLimits(newAngle, newAngle);
	frJoint->SetLimits(newAngle, newAngle);

	double newtireangle = ((newAngle * RADTODEG) / 90);
	//gef::DebugOut("newAngle: %f", newAngle);

	tire_angle = (newtireangle+0.5);

	for (std::vector<Tire*>::size_type it = 0; it != tires.size(); it++)
	{
		tires[it]->updateFriction();
		tires[it]->updateDrive(control_state);
		tires[it]->updateTurn(control_state);
	}
	
	UpdateTime();
	UpdateSprites();

}

void AICar::UpdateNN(std::vector<Waypoint*> wps)
{
	// set up our values for our speed
	float tire_speed = 0;
	for (std::vector<Tire*>::size_type it = 0; it != tires.size(); it++)
	{
		tire_speed += tires[it]->getSpeed();
	}
	speed = ((tire_speed / 4) / 200) + 0.4;

	// set up angles
	for (std::vector<Waypoint*>::size_type it = 0; it != wps.size(); it++)
	{
		if (currentWaypoint == wps[it]->WaypointOrderVal)
		{
			double val = (fmod(wps[it]->body->GetAngle() - fmod((body->GetAngle()), 2 * b2_pi), 2 * b2_pi)) /(2*b2_pi); //*RADTODEG;
			
			if (val < 0)
				val += 1;
			else if (val > 1)
				val -= 1;
			angle_to_waypoint = val;
			gef::DebugOut("WAY ANGLE :%f", wps[it]->body->GetAngle());
			gef::DebugOut("CAR ANGLE :%f", body->GetAngle());
			gef::DebugOut("ANGLE VAL :%f", val);

			waypoint_angle = wps[it]->body->GetAngle();// +(DEGTORAD * 90);//wps[it]->body->GetAngle();//
			//gef::DebugOut("Waypoint angle :%f\n", RADTODEG * waypoint_angle);
			if (waypoint_angle < 0)
			{
				waypoint_angle += 2*b2_pi;
			}
			else if (waypoint_angle > 2 * b2_pi)
			{
				waypoint_angle -= 2 * b2_pi;
			}
		

			gef::DebugOut("Waypoint angle :%f\n", RADTODEG * waypoint_angle);
		}
	}

	// tire angle is set in the update method;

	double inputsignal[4];
	//double inputsignal[2];
	inputsignal[0] = fmod(abs(angle_to_waypoint ), 1);
	inputsignal[1] = distance_to_side;
	inputsignal[2] = speed;
	inputsignal[3] = fmod(abs(tire_angle),1);

	
	gef::DebugOut("Input signals 1:%f 2:%f 3:%f  4:%f \n", inputsignal[0], inputsignal[1], inputsignal[2], inputsignal[3]);

	switch (net_type)
	{
	case EBP:

		
		ebpNN->SetInputSignal(inputsignal);
		ebpNN->PropagateSignal();
		ebpNN->GetOutputSignal(current_control_states);
		break;

	case RPROP:


		rpNN->SetInputSignal(inputsignal);
		rpNN->PropagateSignal();
		rpNN->GetOutputSignal(current_control_states);

		break;

	case RMGSN:

		rmgsNN->SetInputSignal(inputsignal);
		rmgsNN->PropagateSignal();
		rmgsNN->GetOutputSignal(current_control_states);

		break;

	}

	gef::DebugOut("Output signals 1:%f 2:%f 3:%f  4:%f \n", current_control_states[0], current_control_states[1], current_control_states[2], current_control_states[3]);
	gef::DebugOut("WAY : %i\n", currentWaypoint);

}

void AICar::UpdateRaycasts(std::vector<barrier*> bars, b2World* world)
{
	//gef::DebugOut("BodyAngle: %f\n", body->GetAngle());

	LeftSprite.set_rotation(waypoint_angle);
	LeftSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0);

	float rayLength = 100; //long enough to hit the walls
	b2Vec2 p1(body->GetPosition().x, body->GetPosition().y); //center of scene
	b2Vec2 p2 = p1 + rayLength * b2Vec2(cosf(waypoint_angle), sinf(waypoint_angle));//b2Vec2(sinf(waypoint_angle), cosf(waypoint_angle));
	b2Vec2 p3 = p1 - rayLength * b2Vec2(cosf(waypoint_angle), sinf(waypoint_angle));//b2Vec2(sinf(waypoint_angle), cosf(waypoint_angle));

	//set up input
	b2RayCastInput RightCast;
	RightCast.p1 = p1;
	RightCast.p2 = p2;
	RightCast.maxFraction = 1;

	//set up input
	b2RayCastInput LeftCast;
	LeftCast.p1 = p1;
	LeftCast.p2 = p3;
	LeftCast.maxFraction = 1;

	//check every fixture of every body to find closest
	float closestFractionRight = 1; //start with end of line as p2
	b2Vec2 intersectionNormalRight(0, 0);
	//for (std::vector<barrier*>::size_type b = 0; b < bars.size() ; b ++) {
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext()) {
			if (f->GetFilterData().categoryBits == 0x0008)
			{
				b2RayCastOutput output;
				if (!f->RayCast(&output, RightCast, 0))  // for fixtures with a single child (polygon, circel etc) index is 0, chains have more idecies. 
					continue;
				if (output.fraction < closestFractionRight) {
					closestFractionRight = output.fraction;
					intersectionNormalRight = output.normal;
				}
			}
		}
	}
	b2Vec2 intersectionPointRight = p1 + closestFractionRight * (p2 - p1);

	//check every fixture of every body to find closest
	float closestFractionLeft = 1; //start with end of line as p2
	b2Vec2 intersectionNormalLeft(0, 0);
	//for (std::vector<barrier*>::size_type b = 0; b < bars.size(); b++) {
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext()) {
			if (f->GetFilterData().categoryBits == 0x0008)
			{
				b2RayCastOutput output;
				if (!f->RayCast(&output, LeftCast, 0))
					continue;
				if (output.fraction < closestFractionLeft) {
					closestFractionLeft = output.fraction;
					intersectionNormalLeft = output.normal;

				}
			}
			
		}
	}
	b2Vec2 intersectionPointLeft = p1 + closestFractionLeft * (p3 - p1);

	b2Vec2 rl = (intersectionPointRight - p1);
	b2Vec2 ll = (intersectionPointLeft - p1);


	
	float maxdiff = abs(rl.Length()) + abs(ll.Length()); //rl.Length() + ll.Length();
	float sideDiff = abs(maxdiff) - abs(ll.Length()); //rl.Length() -  ll.Length(); // 


	RightHitSprite.set_position(intersectionPointRight.x, intersectionPointRight.y, 0);
	LeftHitSprite.set_position(intersectionPointLeft.x, intersectionPointLeft.y, 0);


	//gef::DebugOut("side diff : %f\n", sideDiff);
	//gef::DebugOut("rl : %f\n", rl.Length());
	//gef::DebugOut("ll : %f\n", ll.Length());



	distance_to_side = (sideDiff / (maxdiff));//(sideDiff / (maxdiff)); // do some maths here to make sure it is between 0 and 1
}

void AICar::draw(gef::SpriteRenderer* sprite_renderer)
{

	sprite_renderer->DrawSprite(carBodySprite);
	sprite_renderer->DrawSprite(LeftSprite);
	sprite_renderer->DrawSprite(RightHitSprite);
	sprite_renderer->DrawSprite(LeftHitSprite);
	sprite_renderer->DrawSprite(UpButton);
	sprite_renderer->DrawSprite(DownButton);
	sprite_renderer->DrawSprite(LeftButton);
	sprite_renderer->DrawSprite(RightButton);


	for (std::vector<Tire*>::size_type it = 0; it != tires.size(); it++)
	{
		tires[it]->draw(sprite_renderer);
	}


	
}

void AICar::SaveWeights()
{
	const char* fname;

	switch (net_type)
	{
	case EBP:

		fname = "ebpweights.txt";

		break;
	case RPROP:

		fname = "rpropweights.txt";

		break;
	case RMGSN:

		fname = "rmgsweights.txt";

		break;

	default:
		break;
	}



	std::ofstream file(fname);
	int i, j, k;
	switch (net_type)
	{
	case EBP:

		for (i = 1; i < ebpNN->num_layers; i++)
		{
			for (j = 0; j < ebpNN->layers[i].num_Neurons; j++)
			{
				for (k = 0; k < ebpNN->layers[i - 1].num_Neurons; k++)
				{
					file << (ebpNN->layers[i].neurons[j].saved_weight[k]);
					file << "\n";
				}
					
					 
			}
		}

		break;

	case RPROP:

		for (i = 1; i < rpNN->num_layers; i++)
		{
			for (j = 0; j < rpNN->layers[i].num_Neurons; j++)
			{
				for (k = 0; k < rpNN->layers[i - 1].num_Neurons; k++)
				{
					file << (rpNN->layers[i].neurons[j].saved_weight[k]);
					file << "\n";
				}
				file << (rpNN->layers[i].neurons[j].savedBias);
				file << "\n";
			}
		}

		break;

	case RMGSN:

		for (i = 1; i < rmgsNN->num_layers; i++)
		{
			for (j = 0; j < rmgsNN->layers[i].num_Neurons; j++)
			{
				for (k = 0; k < rmgsNN->layers[i - 1].num_Neurons; k++)
				{
					file << (rmgsNN->layers[i].neurons[j].weight[k]);
					file << "\n";
				}
			}
		}

		break;

	}

	file.close();


}

void AICar::LoadWeights()
{
	FILE* fp;
	errno_t err;
	const char* fname;

	switch (net_type)
	{
	case EBP:

		fname = "ebpweights.txt";

		break;
	case RPROP:

		fname = "rpropweights.txt";

		break;
	case RMGSN:

		fname = "rmgsweights.txt";

		break;

	default:
		break;
	}

	if ((err = fopen_s(&fp, fname, "r")) != 0)
	{
		printf("couldn't open file");
	}


	int i, j, k;
	switch (net_type)
	{
	case EBP:
		
		for (i = 1; i < ebpNN->num_layers; i++)
		{
			for (j = 0; j < ebpNN->layers[i].num_Neurons; j++)
			{
				for (k = 0; k < ebpNN->layers[i - 1].num_Neurons; k++)
				{
					double dNumber;
					if (read_number(fp, &dNumber))
					{
						ebpNN->layers[i].neurons[j].weight[k] = dNumber;
						gef::DebugOut("WEIGHT[%i][%i][%i]: %f\n", i, j, k, ebpNN->layers[i].neurons[j].weight[k]);
					}
				}
			}
		}

		break;

	case RPROP:
		
		for (i = 1; i < rpNN->num_layers; i++)
		{
			for (j = 0; j < rpNN->layers[i].num_Neurons; j++)
			{
				for (k = 0; k < rpNN->layers[i - 1].num_Neurons; k++)
				{
					double dNumber;
					if (read_number(fp, &dNumber))
					{
						rpNN->layers[i].neurons[j].weight[k] = dNumber;
					}
				}
				double dNumber;
				if (read_number(fp, &dNumber))
				{
					rpNN->layers[i].neurons[j].bias = dNumber;
				}
			}
		}

		break;

	case RMGSN:
		
		for (i = 1; i < rmgsNN->num_layers; i++)
		{
			for (j = 0; j < rmgsNN->layers[i].num_Neurons; j++)
			{
				for (k = 0; k < rmgsNN->layers[i - 1].num_Neurons; k++)
				{
					double dNumber;
					if (read_number(fp, &dNumber))
					{
						rmgsNN->layers[i].neurons[j].weight[k] = dNumber;
					}
				}
			}
		}

		break;

	}

	if (fp) fclose(fp);

	//rmgsNN->ComputeOutputError("traindat42.txt", 10000);

}

void AICar::UpdateButtons()
{
	
	if(current_control_states[0] >= 0.5 && prev_control_states[0] < 0.5)
		control_state |= TDC_LEFT; //control_state |= TDC_RIGHT; //
	else if (current_control_states[0] < 0.5 && prev_control_states[0] >= 0.5)
		control_state &= ~TDC_LEFT; //control_state &= ~TDC_RIGHT; //

	if (current_control_states[1] >= 0.5 && prev_control_states[1] < 0.5)
		control_state |= TDC_RIGHT; //control_state |= TDC_LEFT;//
	else if (current_control_states[1] < 0.5 && prev_control_states[1] >= 0.5)
		control_state &= ~TDC_RIGHT; //control_state &= ~TDC_LEFT;//
	
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

bool AICar::read_number(FILE* fp, double* number)
{
	char szWord[256];
	int i = 0;
	int b;

	*number = 0;

	szWord[0] = '\0';
	while (((b = fgetc(fp)) != EOF) && (i < 255))
	{
		if ((b == '.') ||
			(b == '0') ||
			(b == '1') ||
			(b == '2') ||
			(b == '3') ||
			(b == '4') ||
			(b == '5') ||
			(b == '6') ||
			(b == '7') ||
			(b == '8') ||
			(b == '9') ||
			(b == '-'))
		{
			szWord[i++] = (char)b;
		}
		else
			if (i > 0) break;
	}
	szWord[i] = '\0';

	if (i == 0) return false;

	*number = atof(szWord);

	return true;
}