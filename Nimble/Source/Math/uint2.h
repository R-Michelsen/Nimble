#pragma once

#include <cstdint>

namespace Nimble
{
	struct uint2
	{
		uint32_t x;
		uint32_t y;

		uint2() = default;
		uint2(uint32_t x, uint32_t y) : x(x), y(y) {}

		inline bool operator==(const uint2& other) const
		{
			return (x == other.x) && (y == other.y);
		}

		inline uint2& operator+=(uint2 other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		inline uint2 operator+(uint2 other)
		{
			return uint2(x + other.x, y + other.y);
		}

		inline uint2& operator-=(uint2 other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		inline uint2 operator-(uint2 other)
		{
			return uint2(x - other.x, y - other.y);
		}
	};
}