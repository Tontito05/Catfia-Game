#pragma once

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	double ReadSec() const;
	void Stop();
	void continueTimer();
	float GetTime() const ;
	float ReadMSec() const;
	void RsetTimer();
	bool active;

private:

	// Start time in seconds
	double started_at;
	double paused_at = 0;

	bool time_paused;


};