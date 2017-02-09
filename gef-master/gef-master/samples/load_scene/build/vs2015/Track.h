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
public:
	Track(const char * fname, b2World* world);

	void LoadTrack(FILE* fp);
	void SetUpTrack(b2World* world);
	void SetUpWaypoints(b2World* world);

	void WaypointBuildLetter(barrier* bar, char upper, b2World* world);
	void WaypointBuildNumber(barrier* bar, char higher, b2World* world);

	void DrawTrack(gef::SpriteRenderer* sprite_renderer, bool debug);

	std::vector<barrier*> Barriers;
	std::vector<Waypoint*> WayPoints;

	char** map;

};


#endif // !TRACK_H
