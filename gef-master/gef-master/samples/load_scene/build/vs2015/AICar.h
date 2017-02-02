#ifndef AICAR_H
#define AICAR_H

#include "Car.h"
#include "NeuralNetwork.h"
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
	AICar(b2World* world, Net network);
	void Update();
	void UpdateNN(double* outputs);

	RProp rpNN;
	RMGS rmgsNN;
	BProp ebpNN;

	int control_state;
	float angle_to_waypoint;
	float distance_to_side;
	float speed;
	float tire_angle;

	Net net_type;
};


#endif // !AICAR_H

