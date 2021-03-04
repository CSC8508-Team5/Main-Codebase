#include "Timer.h"

Timer::Timer(){
	is_stop = true;
	is_pause = false;
}

Timer::~Timer() {}

bool Timer::isPause() {
	if (is_pause)
		return true;
	else
	return false;
}

bool Timer::isStop() {
	if (is_stop)
		return true;
	else
		return false;
}

void Timer::Start()
{
	if (is_stop)
	{
		start_time = clock();
		is_stop = false;
	}
	else if (is_pause)
	{
		is_pause = false;
		start_time += clock() - pause_time;
	}
}


void Timer::Pause() 
{
	if (is_stop || is_pause)  
		return;
	else 
	{
		is_pause = true;
		pause_time = clock();
	}
}


void Timer::Stop()
{
	if (is_stop) 
		return;
	else if (is_pause)
	{
		is_pause = false;
		is_stop = true;
		stop_time = clock();
	}
	else if (!is_stop)
	{
		is_stop = true;
		stop_time = clock();
	}
}

double Timer::Passedtime() {
	clock_t time_now = clock();
	passed_time = (double)(time_now - start_time);
	return passed_time;
}

void Timer::show() {}