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
	started_at = GetTime();
	paused_at = 0;
	time_paused = false;
}

double Timer::ReadSec() const
{
	if (time_paused == true)
	{
		return paused_at;
	}
	if (time_paused == false)
	{
		return (GetTime() - started_at + paused_at);
	}
}

void Timer::Stop()
{
	if (time_paused == false)
	{
		paused_at = ReadSec();
	}
	time_paused = true;
}

void Timer::continueTimer()
{
	time_paused = false;
}

float Timer::GetTime() const
{
	return SDL_GetTicks() / 1000.0f;
}

float Timer::ReadMSec() const
{
	float Msecs = (float)(SDL_GetTicks() - started_at);
	return Msecs;
}
void Timer::RsetTimer()
{
	active = false;
}