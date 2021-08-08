#include "Timer.h"

static const long Infinite = -1L;

Timer::Timer(const std::function<void(void)> &f)
		: funct(f)
{ }

Timer::Timer(const std::function<void(void)> &f, const unsigned long &interval, const long repeatNum)
		: funct(f)
		, interval(std::chrono::milliseconds(interval))
		, callNumber(repeatNum)
{ }

void Timer::Start(bool async, bool runOnStartup)
{
	if (IsActive())
		return;
	active = true;
	repeatsLeft = callNumber;

	reverse = runOnStartup;

	if (async)
		thread = std::thread(&Timer::threadCallback, this);
	else
		this->threadCallback();
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

void Timer::sleepAndRun()
{
	std::this_thread::sleep_for(interval);
	if (active)
		GetFunction()();
}

void Timer::runAndSleep()
{
	if (active)
		GetFunction()();

	std::this_thread::sleep_for(interval);
}

void Timer::controlledRun()
{
	if (reverse)
		runAndSleep();
	else
		sleepAndRun();
}

void Timer::threadCallback()
{
	if (callNumber == Infinite)
	{
		// Runs until .Stop() is called
		while (active)
			controlledRun();
	}
	else
	{
		// Runs until repeat count goes to 0
		while (repeatsLeft--)
			controlledRun();
	}
}
