#include <EASTL/chrono.h>

#include <chrono>
namespace Nimble
{
	using clock = eastl::chrono::steady_clock;

	class Time
	{
	public:
		float delta;

		Time() : delta(0.0f)
		{
			start = eastl::chrono::time_point_cast<eastl::chrono::nanoseconds>(clock::now());
		}

		inline void UpdateTimer()
		{
			auto now = clock::now();
			delta = static_cast<eastl::chrono::duration<float, eastl::ratio<1>>>(now - start).count();
			start = now;
		}

	private:
		eastl::chrono::time_point<eastl::chrono::steady_clock> start;
	};
}

