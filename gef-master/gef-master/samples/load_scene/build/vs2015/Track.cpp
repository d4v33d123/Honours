#include "Track.h"
#include "system\debug_log.h"

Track::Track(const char* fname, b2World* world, int number)
{
	FILE* fp;
	errno_t err;

	if ((fp = _fsopen(fname, "r", _SH_DENYNO)) == NULL)
	{
		printf("couldn't open file");
	}
	tracknumber = number;

	LoadTrack(fp);
	fclose(fp);
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
		for (int j = 0; j < 41; j++)
		{
			map[i][j] = char(fgetc(fp));
		}
	}

	// now we need to load in the waypoints order from the file so that we are only accessing the file in one function for efficency

	waypointOrderPositions = new double*[78];
	for (int i = 0; i < 78; i++)
	{
		waypointOrderPositions[i] = new double[2];

	}
	int xVals = 0;
	int yVals = 0;

	while (!feof(fp))
	{
		double number;
		if (read_number(fp, &number))
		{
			if (xVals < 2)
			{
				waypointOrderPositions[yVals][xVals] = (number - 1.0);
				gef::DebugOut("%f ", waypointOrderPositions[yVals][xVals]);
				xVals++;
			}

			if (xVals == 2)
			{
				yVals++;
				xVals = 0;
				gef::DebugOut("\n");
			}
			// success
		}
		else
		{
			//error
			gef::DebugOut("Finished Loading numbers");
			break;
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
				if (map[i][j] == '#')
				{
					Barriers.push_back(new barrier(i, j, world, BARRIERCAT, CARCAT | TIRECAT));
				}
				else
				{
					// if it is anything else, set the vale too, so that we can build waypoints with the information
					Barriers.push_back(new barrier(i, j, world, BARRIERCAT, CARCAT | TIRECAT, map[i][j]));
				}
			}
			printf("%c",map[i][j]);
			gef::DebugOut("%c", map[i][j]);
				
				
		}
		gef::DebugOut("\n");
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
	// we can do this by sorting the vector with using the waypoint val variable
	//at the moment we will just leave them unordered and search for the right one, firx this for efficency
	//std::sort(WayPoints.begin(), WayPoints.end(), CompareWaypoints());

	// for this particular level, we must flip some of the waypoints
	
	if (tracknumber == 1)
	{
		for (std::vector<Waypoint*>::size_type it = 0; it != WayPoints.size(); it++)
		{
			int val = WayPoints[it]->WaypointOrderVal;
			if (val == 21 ||
				val == 22 ||
				val == 23 ||
				val == 24 ||
				val == 25 ||
				val == 26 ||
				val == 27 ||
				val == 28 ||
				val == 29 ||
				val == 30 ||
				val == 31 ||
				val == 32 ||
				val == 33 ||
				val == 46 ||
				val == 47 ||
				val == 48 ||
				val == 49 ||
				val == 50 ||
				val == 51 ||
				val == 52 ||
				val == 64 ||
				val == 65 ||
				val == 66 ||
				val == 67 ||
				val == 68 ||
				val == 69)
			{
				WayPoints[it]->body->SetTransform(WayPoints[it]->body->GetPosition(), WayPoints[it]->body->GetAngle() + b2_pi);
			}

		}
	}
	

}

void Track::WaypointBuildLetter(barrier* bar, char upper, b2World* world)
{
	// check for any barriers with the "uppercase" value and push it onto our waypoints vector
	for (auto it = Barriers.begin(); it != Barriers.end(); it++)
	{
		barrier* bar2 = *it;
		// make sure the barrier isn't equal to the current one
		if (bar2->WaypointVal == upper && bar != bar2)
			WayPoints.push_back(new Waypoint(bar->body->GetPosition().x, bar->body->GetPosition().y, bar2->body->GetPosition().x, bar2->body->GetPosition().y, bar2->mapy, bar2->mapx, world, WAYPOINTCAT, CARCAT, waypointOrderPositions));
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
			WayPoints.push_back(new Waypoint(bar->body->GetPosition().x, bar->body->GetPosition().y, bar2->body->GetPosition().x, bar2->body->GetPosition().y, bar2->mapy, bar2->mapx, world, WAYPOINTCAT, CARCAT, waypointOrderPositions));
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

void Track::UpdateSprites()
{
	for (std::vector<barrier*>::size_type it = 0; it != Barriers.size(); it++)
	{
		Barriers[it]->UpdateSprite();
	}

	for (std::vector<Waypoint*>::size_type it = 0; it != WayPoints.size(); it++)
	{
		WayPoints[it]->UpdateSprite();
	}
	

}

bool Track::read_number(FILE* fp, double* number)
{
	char szWord[256];
	int i = 0;
	int b;

	*number = 0;

	szWord[0] = '\0';
	while (((b = fgetc(fp)) != EOF) && (i < 255))
	{
		if ((b == '.') ||
			(b == '0') ||
			(b == '1') ||
			(b == '2') ||
			(b == '3') ||
			(b == '4') ||
			(b == '5') ||
			(b == '6') ||
			(b == '7') ||
			(b == '8') ||
			(b == '9'))
		{
			szWord[i++] = (char)b;
		}
		else
			if (i > 0) break;
	}
	szWord[i] = '\0';

	if (i == 0) return false;

	*number = atof(szWord);

	return true;
}