#pragma once

#include "src/Cosmic.h"
#include <chrono>

namespace cm
{
	struct TimeResult
	{
		real32 delta_seconds;
		real32 delta_milliseconds;
		real32 delta_microseconds;
	};

	class Clock
	{
	public:
		inline void Start() { this->start_time = std::chrono::steady_clock::now(); }
		inline void End() { this->end_time = std::chrono::steady_clock::now(); }

		inline TimeResult Mark() {
			std::chrono::time_point<std::chrono::steady_clock> current_time = std::chrono::steady_clock::now();
			std::chrono::microseconds elapsed_time(std::chrono::duration_cast<std::chrono::microseconds>(current_time - this->start_time));
			real32 time = static_cast<real32>(elapsed_time.count());

			TimeResult result = {};
			result.delta_microseconds = time;
			result.delta_milliseconds = time * 0.001f;
			result.delta_seconds = time * 0.001f * 0.001f;

			return result;
		}

		inline TimeResult Get() {
			std::chrono::microseconds elapsed_time(std::chrono::duration_cast<std::chrono::microseconds>(this->end_time - this->start_time));
			real32 time = static_cast<real32>(elapsed_time.count());

			TimeResult result = {};
			result.delta_microseconds = time;
			result.delta_milliseconds = time * 0.001f;
			result.delta_seconds = time * 0.001f * 0.001f;

			return result;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> start_time;
		std::chrono::time_point<std::chrono::steady_clock> end_time;
	};

	class ProfilerClock
	{
	public:
		static std::unordered_map<String, TimeResult> time_results;

	public:
		ProfilerClock(const String &name);
		~ProfilerClock();

	private:
		String name;
		Clock clock;
	};

#define PROFILE_FUNCTION() ProfilerClock pclock(__FUNCTION__);

}