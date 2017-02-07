#ifndef AICAR_H
#define AICAR_H

#include <Box2D/Box2D.h>
#include "Car.h"
#include "BackPropagation.h"
#include "RProp.h"
#include "RMGS.h"

enum Net
{
	EBP,
	RPROP,
	RMGSN
};


class AICar : public Car
{
	AICar(b2World* world, Net network, int ds);
	void Train(const char* fname);
	void Update();
	void UpdateNN(double* outputs);
	void UpdateButtons();

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
