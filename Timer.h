#pragma once

#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>
#include <functional>

/**
 * TODO This class needs to be reorganized into two files
 * TODO The constructor also needs to be fixed to initialize private variables
 */
class Timer
{

public:
	static const long Infinite = -1L;

	Timer() { }
	Timer(const std::function<void(void)>& f) : funct(f) { }
	Timer(const std::function<void(void)>& f, const unsigned long& i, const long repeat=Timer::Infinite) 
		: funct(f), interval(std::chrono::milliseconds(i)), CallNumber(repeat) { }

	void Start(bool Async=true)
	{
		if (IsActive())
			return;
		Active = true;
		repeat_count = CallNumber;

		if (Async)
			Thread = std::thread(&Timer::ThreadFunc, this);
		else
			this->ThreadFunc();
	}

	void Stop()
	{
		Active = false;
		Thread.join();
	}

	void SetFunction(const std::function<void(void)>& f)
	{
		funct = f;
	}

	bool IsActive() const { return Active; }
	
	void RepeatCount(const long r)
	{
		if (Active)	return;
		CallNumber = r;
	}

	long GetLeftCount() const { return repeat_count; }

	long RepeatCount() const { return CallNumber; }

	void SetInterval(const unsigned long& i)
	{
		if (Active) return;
		interval = std::chrono::milliseconds(i);
	}

	unsigned long Interval() const { return interval.count(); }

	const std::function<void(void)>& Function() const
	{
		return funct;
	}

private:
	std::thread Thread;
	bool Active = false;
	long CallNumber = -1L;
	long repeat_count = -1L;
	std::chrono::milliseconds interval = std::chrono::milliseconds(0);
	std::function<void(void)> funct = nullptr;

	void SleepAndRun()
	{
		std::this_thread::sleep_for(interval);
		if (Active)
			Function()();
	}

	void ThreadFunc()
	{
		if (CallNumber == Infinite)
			while (Active)
				SleepAndRun();
		else
			while (repeat_count--)
				SleepAndRun();
	}
};

#endif
