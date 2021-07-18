#include "Timer.h"

static const long Infinite = -1L;

Timer::Timer()
{ }

Timer::Timer(const std::function<void(void)> &f)
		: funct(f)
{ }

Timer::Timer(const std::function<void(void)> &f, const unsigned long &interval, const long repeatNum)
		: funct(f), interval(std::chrono::milliseconds(interval)), callNumber(repeatNum)
{ }

void Timer::Start(bool async)
{
	if (IsActive())
		return;
	active = true;
	repeatsLeft = callNumber;

	if (async)
		thread = std::thread(&Timer::ThreadCallback, this);
	else
		this->ThreadCallback();
}

void Timer::Stop()
{
	active = false;
	thread.join();
}

void Timer::SetRepeatCount(const long r)
{
	if (active)
		return;
	callNumber = r;
}

void Timer::SetInterval(const unsigned long &i)
{
	if (active)
		return;
	interval = std::chrono::milliseconds(i);
}

void Timer::SleepAndRun()
{
	std::this_thread::sleep_for(interval);
	if (active)
		GetFunction()();
}

void Timer::ThreadCallback()
{
	if (callNumber == Infinite)
	{
		// Runs until .Stop() is called
		while (active)
			SleepAndRun();
	}
	else
	{
		// Runs until repeat count goes to 0
		while (repeatsLeft--)
			SleepAndRun();
	}
}
