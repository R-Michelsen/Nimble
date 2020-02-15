#pragma once

namespace Nimble
{
	struct float2
	{
		float x;
		float y;

		float2() = default;
		float2(float x, float y) : x(x), y(y) {}

		inline bool operator==(const float2& other) const
		{
			return (x == other.x) && (y == other.y);
		}

		inline float2& operator+=(float2 other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		inline float2 operator+(float2 other)
		{
			return float2(x + other.x, y + other.y);
		}

		inline float2 operator+(const float2 other) const
		{
			return float2(x + other.x, y + other.y);
		}

		inline float2& operator-=(float2 other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		inline float2 operator-(float2 other)
		{
			return float2(x - other.x, y - other.y);
		}

		inline float2 operator-(const float2 other) const
		{
			return float2(x - other.x, y - other.y);
		}

		inline float2& operator*=(float s)
		{
			x *= s;
			y *= s;
			return *this;
		}

		inline float2 operator*(float s)
		{
			return float2(x * s, y * s);
		}

		inline float2 operator/=(float s)
		{
			x /= s;
			y /= s;
			return *this;
		}

		inline float2 operator/(float s)
		{
			return float2(x / s, y / s);
		}
	};
}