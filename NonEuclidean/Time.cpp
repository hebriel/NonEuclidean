#include "Time.hpp"

Time::Time()
{

}

Time::Time(std::chrono::duration<long long, std::ratio<1,1000000000>> standardDuration)
{
	m_duration = standardDuration;
}

Time Time::seconds(long double s)
{
	Time time;
	time.setStdDuration(std::chrono::nanoseconds(static_cast<uint64_t>(s*1000000000)));
	return time;
}

Time Time::milliseconds(long double s)
{
	Time time;
	time.setStdDuration(std::chrono::nanoseconds(static_cast<uint64_t>(s*1000000)));
	return time;
}

Time Time::microseconds(long double s)
{
	Time time;
	time.setStdDuration(std::chrono::nanoseconds(static_cast<uint64_t>(s*1000)));
	return time;
}

Time Time::nanoseconds(uint64_t s)
{
	Time time;
	time.setStdDuration(std::chrono::nanoseconds(s));
	return time;
}

long long Time::asNanoseconds() const
{
	return m_duration.count();
}

long double Time::asMicroseconds() const
{
	return m_duration.count()/1000.0;
}

long double Time::asMilliseconds() const
{
	return m_duration.count()/1000000.0;
}

long double Time::asSeconds() const
{
	return m_duration.count()/1000000000.0;
}

bool Time::operator==(const Time& time) const
{
	return asNanoseconds() == time.asNanoseconds();
}

bool Time::operator<(const Time& time) const
{
	return asNanoseconds() < time.asNanoseconds();
}

bool Time::operator<=(const Time& time) const
{
	return asNanoseconds() <= time.asNanoseconds();
}

bool Time::operator>(const Time& time) const
{
	return asNanoseconds() > time.asNanoseconds();
}

bool Time::operator>=(const Time& time) const
{
	return asNanoseconds() >= time.asNanoseconds();
}

Time Time::operator+=(const Time& time)
{
	Time t = nanoseconds(asNanoseconds() + time.asNanoseconds());
	return t;
}

Time Time::operator-=(const Time& time)
{
	Time t = nanoseconds(asNanoseconds() - time.asNanoseconds());
	return t;
}

void Time::setStdDuration(std::chrono::duration<long long, std::ratio<1,1000000000>> duration)
{
	m_duration = duration;
}

std::chrono::duration<long long, std::ratio<1,1000000000>> Time::getStdDuration() const
{
	return m_duration;
}

Clock::Clock() : m_start(std::chrono::high_resolution_clock::now())
{

}

Time Clock::restart()
{
	m_start = std::chrono::high_resolution_clock::now();
	return getElapsedTime();
}

Time Clock::getElapsedTime() const
{
	return std::chrono::high_resolution_clock::now() - m_start;
}
