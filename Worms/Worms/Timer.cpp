#include "Timer.h"



Timer::Timer()
{
	this->getStartValue = false;
	this->getStopValue = false;

	this->start();

}


Timer::~Timer()
{
}

void Timer::start()
{
	this->startValue = std::chrono::high_resolution_clock::now(); //Neat use for std lib
	this->getStartValue = true;
}

void Timer::stop()
{
	this->stopValue = std::chrono::high_resolution_clock::now();
	this->getStopValue = true;
}

float Timer::getTime()
{
	float retValue = TIMERERROR;
	if (this->getStartValue && this->getStopValue)
	{
		this->timePassed = this->stopValue - this->startValue;
		retValue = this->timePassed.count() * 1000;
	}
	return retValue;
}
