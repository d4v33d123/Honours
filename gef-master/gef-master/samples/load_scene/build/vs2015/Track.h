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

enum CollisionCategories
{
	CARCAT = 0x0001,
	TIRECAT = 0x0002,
	WAYPOINTCAT = 0x0004,
	BARRIERCAT = 0x0008
};

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
	void UpdateSprites();

	std::vector<barrier*> Barriers;
	std::vector<Waypoint*> WayPoints;

	char** map;

};


#endif // !TRACK_H
