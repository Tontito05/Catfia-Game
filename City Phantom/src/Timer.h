#pragma once

class Timer
{
public:

	// Constructor
	Timer();
	bool active = false;

	void Start();
	int ReadSec() const;
	float ReadMSec() const;
	void RsetTimer();

private:
	int startTime;

};