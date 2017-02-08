#include "Track.h"



void Track::LoadTrack(FILE* fp)
{
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
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			if (map[i][j] != '-')
			{
				if (map[i][j] = '#')
				{
					Barriers.push_back(new barrier(i, j, world));
				}
				else
				{
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
		if (bar->WaypointVal != '#')
		{
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
}

void Track::WaypointBuildLetter(barrier* bar, char higher, b2World* world)
{
	for (auto it = Barriers.begin(); it != Barriers.end(); it++)
	{
		barrier* bar2 = *it;
		if (bar2->WaypointVal == higher)
			WayPoints.push_back(new Waypoint(bar->body->GetPosition().x, bar->body->GetPosition().y, bar2->body->GetPosition().x, bar2->body->GetPosition().y, world, higher));
	}
}

void Track::WaypointBuildNumber(barrier * bar, char higher, b2World * world)
{
	for (auto it = Barriers.begin(); it != Barriers.end(); it++)
	{
		barrier* bar2 = *it;
		if (bar2->WaypointVal == higher && bar != bar2)
			WayPoints.push_back(new Waypoint(bar->body->GetPosition().x, bar->body->GetPosition().y, bar2->body->GetPosition().x, bar2->body->GetPosition().y, world, higher));
	}
}
