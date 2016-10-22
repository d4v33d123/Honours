#ifndef TIRE_H
#define TIRE_H

#include <Box2D/Box2D.h>
#include <set>
#include <vector>
#include <graphics/sprite.h>
#include <graphics/sprite_renderer.h>

enum {
	TDC_LEFT = 0x1,
	TDC_RIGHT = 0x2,
	TDC_UP = 0x4,
	TDC_DOWN = 0x8
};




//types of fixture user data
enum fixtureUserDataType {
	FUD_CAR_TIRE,
	FUD_GROUND_AREA
};

//a class to allow subclassing of different fixture user data
class FixtureUserData {
	fixtureUserDataType m_type;
protected:
	FixtureUserData(fixtureUserDataType type) : m_type(type) {}
public:
	virtual fixtureUserDataType getType() { return m_type; }
	virtual ~FixtureUserData() {}
};

//class to allow marking a fixture as a car tire
class CarTireFUD : public FixtureUserData {
public:
	CarTireFUD() : FixtureUserData(FUD_CAR_TIRE) {}
};

//class to allow marking a fixture as a ground area
class GroundAreaFUD : public FixtureUserData {
public:
	float frictionModifier;
	bool outOfCourse;

	GroundAreaFUD(float fm, bool ooc) : FixtureUserData(FUD_GROUND_AREA) {
		frictionModifier = fm;
		outOfCourse = ooc;
	}
};

class Tire
{
public:
	b2Body* body;
	float maxSpeed;
	float maxRSpeed;
	float maxDriveForce;
	float maxLateralImpulse;
	std::set<GroundAreaFUD*> groundAreas;
	float traction;
	gef::Sprite tireSprite;

	Tire(b2World* world);
	~Tire();
	void setCharacteristics(float maxForwardSpeed, float maxBackwardSpeed, float maxDriveForce, float maxLateralImpulse);

	void addGroundArea(GroundAreaFUD* ga) { groundAreas.insert(ga); updateTraction(); }
	void removeGroundArea(GroundAreaFUD* ga) { groundAreas.erase(ga); updateTraction(); }

	void updateTraction();

	b2Vec2 getLateralVelocity();

	b2Vec2 getForwardVelocity();

	void updateFriction();

	void updateDrive(int controlState);

	void updateTurn(int controlState);

	void UpdateSprite();
	
	void draw(gef::SpriteRenderer* sprite_renderer);
};


#endif

/*
class TDTire {
public:
b2Body* m_body;
float m_maxForwardSpeed;
float m_maxBackwardSpeed;
float m_maxDriveForce;
float m_maxLateralImpulse;
std::set<GroundAreaFUD*> m_groundAreas;
float m_currentTraction;

TDTire(b2World* world) {
b2BodyDef bodyDef;
bodyDef.type = b2_dynamicBody;
m_body = world->CreateBody(&bodyDef);

b2PolygonShape polygonShape;
polygonShape.SetAsBox( 0.5f, 1.25f );
b2Fixture* fixture = m_body->CreateFixture(&polygonShape, 1);//shape, density
fixture->SetUserData( new CarTireFUD() );

m_body->SetUserData( this );

m_currentTraction = 1;
}

~TDTire() {
m_body->GetWorld()->DestroyBody(m_body);
}

void setCharacteristics(float maxForwardSpeed, float maxBackwardSpeed, float maxDriveForce, float maxLateralImpulse) {
m_maxForwardSpeed = maxForwardSpeed;
m_maxBackwardSpeed = maxBackwardSpeed;
m_maxDriveForce = maxDriveForce;
m_maxLateralImpulse = maxLateralImpulse;
}

void addGroundArea(GroundAreaFUD* ga) { m_groundAreas.insert(ga); updateTraction(); }
void removeGroundArea(GroundAreaFUD* ga) { m_groundAreas.erase(ga); updateTraction(); }

void updateTraction()
{
if ( m_groundAreas.empty() )
m_currentTraction = 1;
else {
//find area with highest traction
m_currentTraction = 0;
std::set<GroundAreaFUD*>::iterator it = m_groundAreas.begin();
while (it != m_groundAreas.end()) {
GroundAreaFUD* ga = *it;
if ( ga->frictionModifier > m_currentTraction )
m_currentTraction = ga->frictionModifier;
++it;
}
}
}

b2Vec2 getLateralVelocity() {
b2Vec2 currentRightNormal = m_body->GetWorldVector( b2Vec2(1,0) );
return b2Dot( currentRightNormal, m_body->GetLinearVelocity() ) * currentRightNormal;
}

b2Vec2 getForwardVelocity() {
b2Vec2 currentForwardNormal = m_body->GetWorldVector( b2Vec2(0,1) );
return b2Dot( currentForwardNormal, m_body->GetLinearVelocity() ) * currentForwardNormal;
}

void updateFriction() {
//lateral linear velocity
b2Vec2 impulse = m_body->GetMass() * -getLateralVelocity();
if ( impulse.Length() > m_maxLateralImpulse )
impulse *= m_maxLateralImpulse / impulse.Length();
m_body->ApplyLinearImpulse( m_currentTraction * impulse, m_body->GetWorldCenter() );

//angular velocity
m_body->ApplyAngularImpulse( m_currentTraction * 0.1f * m_body->GetInertia() * -m_body->GetAngularVelocity() );

//forward linear velocity
b2Vec2 currentForwardNormal = getForwardVelocity();
float currentForwardSpeed = currentForwardNormal.Normalize();
float dragForceMagnitude = -2 * currentForwardSpeed;
m_body->ApplyForce( m_currentTraction * dragForceMagnitude * currentForwardNormal, m_body->GetWorldCenter() );
}

void updateDrive(int controlState) {

//find desired speed
float desiredSpeed = 0;
switch ( controlState & (TDC_UP|TDC_DOWN) ) {
case TDC_UP:   desiredSpeed = m_maxForwardSpeed;  break;
case TDC_DOWN: desiredSpeed = m_maxBackwardSpeed; break;
default: return;//do nothing
}

//find current speed in forward direction
b2Vec2 currentForwardNormal = m_body->GetWorldVector( b2Vec2(0,1) );
float currentSpeed = b2Dot( getForwardVelocity(), currentForwardNormal );

//apply necessary force
float force = 0;
if ( desiredSpeed > currentSpeed )
force = m_maxDriveForce;
else if ( desiredSpeed < currentSpeed )
force = -m_maxDriveForce;
else
return;
m_body->ApplyForce( m_currentTraction * force * currentForwardNormal, m_body->GetWorldCenter() );
}

void updateTurn(int controlState) {
float desiredTorque = 0;
switch ( controlState & (TDC_LEFT|TDC_RIGHT) ) {
case TDC_LEFT:  desiredTorque = 15;  break;
case TDC_RIGHT: desiredTorque = -15; break;
default: ;//nothing
}
m_body->ApplyTorque( desiredTorque );
}
};
*/