#pragma once

#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>
#include <functional>

class Timer
{

public:
	static const long Infinite = -1L;

	Timer() { }
	Timer(const std::function<void(void)> &f);
	Timer(const std::function<void(void)> &f, const unsigned long &interval, const long repeat=Timer::Infinite);

	/**
	 * Starts running task specified amout of times
	 * every "interval" often 
	 * 
	 * Runs asynchronously if true is passed as arg
	 **/
	void Start(bool async=true);
	void Stop();

	/**
	 * Setters
	 **/
	void SetFunction(const std::function<void(void)>& f) { funct = f; }
	void SetRepeatCount(const long r);
	void SetInterval(const unsigned long& i);

	/**
	 * Getters
	 **/
	long GetRepeatCount() const { return callNumber; }
	long GetNumRepeatsLeft() const { return repeatsLeft; }
	unsigned long GetInterval() const { return interval.count(); }
	const std::function<void(void)>& GetFunction() const { return funct; }

	bool Timer::IsActive() const { return active; }

private:
	std::thread thread;
	bool active = false;

	// Initial number of times to call funct
	long callNumber = -1L;

	// Times left for funct to be called
	long repeatsLeft = -1L;

	// How often funct is called in milliseconds
	std::chrono::milliseconds interval = std::chrono::milliseconds(0);

	// Function that is called by timer
	std::function<void(void)> funct = nullptr;

	/**
	 * Waits until interval is reached and then runs
	 * funct. This repeats until repeatCount becomes 0
	 **/
	void SleepAndRun();

	// Function called when thread is run
	void ThreadCallback();
};

#endif