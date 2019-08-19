#pragma once

/**
 * This 'Time' and 'Clock' implementation is the one from
 * the 'Eyax Time' project. Protected by the
 * Mozilla Public License 2.0
 * https://gitlab.com/eyax/time/
 */

#include <chrono>

class Time
{
public:

	Time();

	Time(std::chrono::duration<long long, std::ratio<1,1000000000>> standardDuration);

	static Time seconds(long double s);
	static Time milliseconds(long double s);
	static Time microseconds(long double s);
	static Time nanoseconds(uint64_t s);

	long long asNanoseconds() const;
	long double asMicroseconds() const;
	long double asMilliseconds() const;
	long double asSeconds() const;

	bool operator==(const Time& time) const;
	bool operator<(const Time& time) const;
	bool operator<=(const Time& time) const;
	bool operator>(const Time& time) const;
	bool operator>=(const Time& time) const;
	Time operator+=(const Time& time);
	Time operator-=(const Time& time);

	void setStdDuration(std::chrono::duration<long long, std::ratio<1,1000000000>> duration);
	std::chrono::duration<long long, std::ratio<1,1000000000>> getStdDuration() const;

private:

	std::chrono::duration<long long, std::ratio<1,1000000000>> m_duration;
};

class Clock
{
public:

	Clock();
	Time restart();
	Time getElapsedTime() const;

private:

	std::chrono::high_resolution_clock::time_point m_start;
};

