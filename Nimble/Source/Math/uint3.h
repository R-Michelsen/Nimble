#pragma once

#include <cstdint>

namespace Nimble
{
	struct uint3
	{
		uint32_t x;
		uint32_t y;
		uint32_t z;

		uint3() = default;
		uint3(uint32_t x, uint32_t y, uint32_t z) : x(x), y(y), z(z) {}

		inline uint3& operator+=(uint3 other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		inline uint3 operator+(uint3 other)
		{
			return uint3(x + other.x, y + other.y, z+ other.z);
		}

		inline uint3& operator-=(uint3 other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		inline uint3 operator-(uint3 other)
		{
			return uint3(x - other.x, y - other.y, z - other.z);
		}
	};
}