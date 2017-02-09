#include "Track.h"

Track::Track(const char* fname, b2World* world)
{
	FILE* fp;
	errno_t err;

	if ((fp = _fsopen(fname, "r", _SH_DENYNO)) == NULL)
	{
		printf("couldn't open file");
	}

	LoadTrack(fp);
	SetUpTrack(world);
	SetUpWaypoints(world);

}

void Track::LoadTrack(FILE* fp)
{
	// run through the file and fill our map with the information 
	map = new char*[40];

	for (int i = 0; i < 40; i++)
	{
		map[i] = new char[40];
		for (int j = 0; j < 40; j++)
		{
			map[i][j] = char(fgetc(fp));
		}
	}

}

void Track::SetUpTrack(b2World * world)
{
	// run through our map matrix and build any barriers in the map
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			if (map[i][j] != '-')
			{
				// if the barrier is a '#' make a regular barrier
				if (map[i][j] = '#')
				{
					Barriers.push_back(new barrier(i, j, world));
				}
				else
				{
					// if it is anything else, set the vale too, so that we can build waypoints with the information
					Barriers.push_back(new barrier(i, j, world, map[i][j]));
				}
			}
				
				
		}
	}


}

void Track::SetUpWaypoints(b2World* world)
{
	for (auto it = Barriers.begin(); it != Barriers.end(); it++)
	{
		barrier* bar = *it;
		// big annoying case statement checking for any letters or numbers, not quite sure how to make this look nicer
		if (bar->WaypointVal != '#')
		{
			// '#' is just a regular barrier, check through the lower case letters as there is only 1 of each in the map
			// for each lowercase letter, check for any upper case letters and build a waypoint
			// check for any numbers and build a waypoint between matching numbers
			switch (bar->WaypointVal)
			{
			case 'a':
				WaypointBuildLetter(bar, 'A', world);
				break;
			case 'b':
				WaypointBuildLetter(bar, 'B', world);
				break;
			case 'c':
				WaypointBuildLetter(bar, 'C', world);
				break;
			case 'd':
				WaypointBuildLetter(bar, 'D', world);
				break;
			case 'e':
				WaypointBuildLetter(bar, 'E', world);
				break;
			case 'f':
				WaypointBuildLetter(bar, 'F', world);
				break;
			case 'g':
				WaypointBuildLetter(bar, 'G', world);
				break;
			case 'h':
				WaypointBuildLetter(bar, 'H', world);
				break;
			case 'i':
				WaypointBuildLetter(bar, 'I', world);
				break;
			case 'j':
				WaypointBuildLetter(bar, 'J', world);
				break;
			case 'k':
				WaypointBuildLetter(bar, 'K', world);
				break;
			case 'l':
				WaypointBuildLetter(bar, 'L', world);
				break;
			case 'm':
				WaypointBuildLetter(bar, 'M', world);
				break;
			case 'n':
				WaypointBuildLetter(bar, 'N', world);
				break;
			case 'o':
				WaypointBuildLetter(bar, 'O', world);
				break;
			case 'p':
				WaypointBuildLetter(bar, 'P', world);
				break;
			case 'q':
				WaypointBuildLetter(bar, 'Q', world);
				break;
			case 'r':
				WaypointBuildLetter(bar, 'R', world);
				break;
			case 's':
				WaypointBuildLetter(bar, 'S', world);
				break;
			case 't':
				WaypointBuildLetter(bar, 'T', world);
				break;
			case 'u':
				WaypointBuildLetter(bar, 'U', world);
				break;
			case 'v':
				WaypointBuildLetter(bar, 'V', world);
				break;
			case 'w':
				WaypointBuildLetter(bar, 'W', world);
				break;
			case 'x':
				WaypointBuildLetter(bar, 'X', world);
				break;
			case 'y':
				WaypointBuildLetter(bar, 'Y', world);
				break;
			case 'z':
				WaypointBuildLetter(bar, 'Z', world);
				break;
			case '0':
				WaypointBuildNumber(bar, '0', world);
				break;
			case '1':
				WaypointBuildNumber(bar, '1', world);
				break;
			case '2':
				WaypointBuildNumber(bar, '2', world);
				break;
			case '3':
				WaypointBuildNumber(bar, '3', world);
				break;
			case '4':
				WaypointBuildNumber(bar, '4', world);
				break;
			case '5':
				WaypointBuildNumber(bar, '5', world);
				break;
			case '6':
				WaypointBuildNumber(bar, '6', world);
				break;
			case '7':
				WaypointBuildNumber(bar, '7', world);
				break;
			case '8':
				WaypointBuildNumber(bar, '8', world);
				break;
			case '9':
				WaypointBuildNumber(bar, '9', world);
				break;
			default:
				break;
			}
		}
	}

	// once we have built all the waypoints, we must order them.
	// there are a number of ways this could be achieved
	// 1. we could put some kind of key in our txt file, but this would jepordise the easy 40x40 structure we have laid out
	// 2. we could order them in alpabetical order, however this becomes a problem when multiple waypoints use the same letter or we come
	//	  across the numbered waypoints, which can be placed anywhere. This also becomes an issue if we do not put waypoints in order, a->c->s...etc
	// 3. we search the map for the closest waypoint, this can be an issue if the track loops back on itself or the waypoints are too infrequent.
	//
	// option 3 will be used as the other two have bigger flaws than it.


}

void Track::WaypointBuildLetter(barrier* bar, char upper, b2World* world)
{
	// check for any barriers with the "uppercase" value and push it onto our waypoints vector
	for (auto it = Barriers.begin(); it != Barriers.end(); it++)
	{
		barrier* bar2 = *it;
		// make sure the barrier isn't equal to the current one
		if (bar2->WaypointVal == upper && bar != bar2)
			WayPoints.push_back(new Waypoint(bar->body->GetPosition().x, bar->body->GetPosition().y, bar2->body->GetPosition().x, bar2->body->GetPosition().y, world, upper));
	}
}

void Track::WaypointBuildNumber(barrier* bar, char higher, b2World * world)
{
	// check for any barriers with the "higher" char value and push it onto our waypoints vector
	for (auto it = Barriers.begin(); it != Barriers.end(); it++)
	{
		barrier* bar2 = *it;
		// make sure the barrier isn't equal to the current one
		if (bar2->WaypointVal == higher && bar != bar2)
			WayPoints.push_back(new Waypoint(bar->body->GetPosition().x, bar->body->GetPosition().y, bar2->body->GetPosition().x, bar2->body->GetPosition().y, world, higher));
	}
}

void Track::DrawTrack(gef::SpriteRenderer* sprite_renderer, bool debug)
{
	// draw the track
	for (std::vector<barrier*>::size_type it = 0; it != Barriers.size(); it++)
	{
		Barriers[it]->draw(sprite_renderer);
	}
	// if we are debugging draw the waypoints
	if (debug)
	{
		for (std::vector<Waypoint*>::size_type it = 0; it != WayPoints.size(); it++)
		{
			WayPoints[it]->draw(sprite_renderer);
		}
	}
	

}
