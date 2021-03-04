#pragma once
#include <time.h>


class Timer {
private:
	clock_t start_time;
	clock_t pause_time;
	clock_t stop_time;
	double passed_time;

	bool is_pause;
	bool is_stop;

public:
	Timer();
	~Timer();
	bool isPause();
	bool isStop();
	void Start();
	void Pause();
	void Stop();
	double Passedtime();
	inline clock_t getStartTime() { return start_time; }
	void show();
};