// ----------------------------------------------------
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "Timer.h"
#include "SDL2\SDL_timer.h"
	
Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	startTime = SDL_GetTicks();
}

int Timer::ReadSec() const
{ 
	int secs = (SDL_GetTicks() - startTime) / 1000;
	return secs;
}

float Timer::ReadMSec() const
{
	float Msecs = (float)(SDL_GetTicks() - startTime);
	return Msecs;
}