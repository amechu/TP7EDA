#pragma once
#include <chrono>


#define TIMERERROR (-1)

class Timer
{
public:
	Timer();
	~Timer();
	void start();
	void stop();
	float getTime(); // In ms
private:
	std::chrono::high_resolution_clock::time_point  startValue;
	bool getStartValue;
	std::chrono::high_resolution_clock::time_point  stopValue;
	bool getStopValue;
	std::chrono:: duration<float> timePassed;
};

