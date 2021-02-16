#include "Clock.h"
namespace cm
{
	std::unordered_map<String, TimeResult> ProfilerClock::time_results;

	ProfilerClock::ProfilerClock(const String &name)
	{
		this->name = name;
		clock.Start();
	}

	ProfilerClock::~ProfilerClock()
	{
		clock.End();
		time_results[name] = clock.Get();
		//std::cout << name << ": " << (clock.Get().delta_milliseconds) << " ms" << std::endl;
	}



}