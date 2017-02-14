#include "Tire.h"

Tire::Tire(b2World* world, uint16 categoryBits, uint16 maskBits)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(0.5f, 1.25f);
	b2FixtureDef fixturedef;
	fixturedef.shape = &polygonShape;
	fixturedef.density = 1;
	fixturedef.filter.categoryBits = categoryBits;
	fixturedef.filter.maskBits = maskBits;
	b2Fixture* fixture = body->CreateFixture(&fixturedef);//shape, density
	fixture->SetUserData(new CarTireFUD());

	body->SetUserData(this);

	traction = 1;

	tireSprite.set_width(0.5);
	tireSprite.set_height(1.25);
	tireSprite.set_colour(0xf01fffff);
	tireSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	tireSprite.set_rotation(-body->GetAngle());


}


Tire::~Tire()
{

	body->GetWorld()->DestroyBody(body);
}

void Tire::setCharacteristics(float nMaxSpeed, float nMaxRSpeed, float nMaxDriveForce, float nMaxLateralImpulse)
{
	maxSpeed = nMaxSpeed;
	maxRSpeed = nMaxRSpeed;
	maxDriveForce = nMaxDriveForce;
	maxLateralImpulse = nMaxLateralImpulse;
}


void Tire::updateTraction()
{
	if (groundAreas.empty())
		traction = 1;
	else {
		//find area with highest traction
		traction = 0;
		std::set<GroundAreaFUD*>::iterator it = groundAreas.begin();
		while (it != groundAreas.end()) {
			GroundAreaFUD* ga = *it;
			if (ga->frictionModifier > traction)
				traction = ga->frictionModifier;
			++it;
		}
	}
}

b2Vec2 Tire::getLateralVelocity() {
	b2Vec2 currentRightNormal = body->GetWorldVector(b2Vec2(1, 0));
	return b2Dot(currentRightNormal, body->GetLinearVelocity()) * currentRightNormal;
}

b2Vec2 Tire::getForwardVelocity() {
	b2Vec2 currentForwardNormal = body->GetWorldVector(b2Vec2(0, 1));
	return b2Dot(currentForwardNormal, body->GetLinearVelocity()) * currentForwardNormal;
}

void Tire::updateFriction() {
	//lateral linear velocity
	b2Vec2 impulse = body->GetMass() * -getLateralVelocity();
	if (impulse.Length() > maxLateralImpulse)
		impulse *= maxLateralImpulse / impulse.Length();
	body->ApplyLinearImpulse(traction * impulse, body->GetWorldCenter(), true);

	//angular velocity
	body->ApplyAngularImpulse(traction * 0.1f * body->GetInertia() * -body->GetAngularVelocity(), true);

	//forward linear velocity
	b2Vec2 currentForwardNormal = getForwardVelocity();
	float currentForwardSpeed = currentForwardNormal.Normalize();
	float dragForceMagnitude = -2 * currentForwardSpeed;
	body->ApplyForce(traction * dragForceMagnitude * currentForwardNormal, body->GetWorldCenter(), true);
}

void Tire::updateDrive(int controlState) {

	//find desired speed
	float desiredSpeed = 0;
	switch (controlState & (TDC_UP | TDC_DOWN)) {
	case TDC_UP:   desiredSpeed = maxSpeed;  break;
	case TDC_DOWN: desiredSpeed = maxRSpeed; break;
	default: return;//do nothing
	}

	//find current speed in forward direction
	b2Vec2 currentForwardNormal = body->GetWorldVector(b2Vec2(0, 1));
	float currentSpeed = b2Dot(getForwardVelocity(), currentForwardNormal);

	//apply necessary force
	float force = 0;
	if (desiredSpeed > currentSpeed)
		force = maxDriveForce;
	else if (desiredSpeed < currentSpeed)
		force = -maxDriveForce;
	else
		return;
	body->ApplyForce(traction * force * currentForwardNormal, body->GetWorldCenter(), true);
}

void Tire::updateTurn(int controlState) {
	float desiredTorque = 0;
	switch (controlState & (TDC_LEFT | TDC_RIGHT)) {
	case TDC_LEFT:  desiredTorque = 15;  break;
	case TDC_RIGHT: desiredTorque = -15; break;
	default:;//nothing
	}
	body->ApplyTorque(desiredTorque, true);
}

void Tire::UpdateSprite()
{
	tireSprite.set_position(body->GetPosition().x, body->GetPosition().y, 0.0f);
	tireSprite.set_rotation(body->GetAngle());
}

void Tire::draw(gef::SpriteRenderer* sprite_renderer)
{
	sprite_renderer->DrawSprite(tireSprite);
}

float Tire::getSpeed()
{
	b2Vec2 currentForwardNormal = body->GetWorldVector(b2Vec2(0, 1));
	float currentSpeed = b2Dot(getForwardVelocity(), currentForwardNormal);

	return currentSpeed;
}