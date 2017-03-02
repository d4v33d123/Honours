#ifndef AICAR_H
#define AICAR_H

#include <Box2D/Box2D.h>
#include "Car.h"
#include "BackPropagation.h"
#include "RProp.h"
#include "RMGS.h"
#include "Waypoint.h"

enum Net
{
	EBP,
	RPROP,
	RMGSN
};


class AICar : public Car
{
public:

	AICar(b2World* world, Net network, int ds, uint16 categoryBits, uint16 maskBits, uint16 tirecategoryBits, uint16 tiremaskBits);
	void Train(const char* fname);
	void Update(std::vector<Waypoint*> wps);
	void UpdateNN(std::vector<Waypoint*> wps);
	void UpdateButtons();
	void draw(gef::SpriteRenderer* sprite_renderer);

	RProp* rpNN;
	RMGS* rmgsNN;
	BProp* ebpNN;

	int control_state;
	int dataSize;
	double current_control_states[4];
	double prev_control_states[4];
	float angle_to_waypoint;
	float distance_to_side;
	float speed;
	float tire_angle;


	Net net_type;
};


#endif // !AICAR_H

