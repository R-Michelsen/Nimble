#pragma once

namespace Nimble
{
	struct float3
	{
		union { float x; float r; };
		union { float y; float g; };
		union { float z; float b; };

		float3() = default;
		float3(float x, float y, float z) : x(x), y(y), z(z) {}

		inline float3& operator+=(float3 other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		inline float3 operator+(float3 other)
		{
			return float3(x + other.x, y + other.y, z + other.z);
		}

		inline float3& operator-=(float3 other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		inline float3 operator-(float3 other)
		{
			return float3(x - other.x, y - other.y, z - other.z);
		}

		inline float3 operator-(const float3 other) const
		{
			return float3(x - other.x, y - other.y, z - other.z);
		}

		inline float3& operator*=(float s)
		{
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		inline float3 operator*(float s)
		{
			return float3(x * s, y * s, z * s);
		}

		inline float3 operator/=(float s)
		{
			x /= s;
			y /= s;
			z /= s;
			return *this;
		}

		inline float3 operator/(float s)
		{
			return float3(x / s, y / s, z / s);
		}
	};
}