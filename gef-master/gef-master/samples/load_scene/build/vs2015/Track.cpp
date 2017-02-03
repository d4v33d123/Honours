#include "Track.h"



void Track::LoadTrack()
{


}

bool Track::read_barrier(FILE* fp, barrier* bar)
{
	int i = 0;
	int b;

	char val;

	
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

		}
		else
			if (i > 0) break;
	}
	szWord[i] = '\0';

	if (i == 0) return false;



	return true;
}