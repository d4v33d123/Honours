#pragma once
#ifndef TRACK_H
#define TRACK_H

#include <Box2D/Box2D.h>
#include "Barrier.h"
#include "Waypoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <graphics/sprite.h>
#include <graphics/sprite_renderer.h>


class Track
{
	Track(const char * fname);

	void LoadTrack();
	
	bool read_barrier(FILE* fp, barrier* b);

	std::vector<barrier*> Barriers;
	std::vector<Waypoint*> WayPoints;

};


#endif // !TRACK_H
