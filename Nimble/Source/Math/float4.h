#pragma once

namespace Nimble
{
	struct float4
	{
		union { float x; float r; };
		union { float y; float g; };
		union { float z; float b; };
		union { float w; float a; };

		float4() = default;
		float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		inline float4& operator+=(float4 other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		inline float4 operator+(float4 other)
		{
			return float4(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		inline float4 operator+(const float4 other) const
		{
			return float4(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		inline float4& operator-=(float4 other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}

		inline float4 operator-(float4 other)
		{
			return float4(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		inline float4 operator-(const float4 other) const
		{
			return float4(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		inline float4& operator*=(float s)
		{
			x *= s;
			y *= s;
			z *= s;
			w *= s;
			return *this;
		}

		inline float4 operator*(float s)
		{
			return float4(x * s, y * s, z * s, w * s);
		}

		inline float4 operator/=(float s)
		{
			x /= s;
			y /= s;
			z /= s;
			w /= s;
			return *this;
		}

		inline float4 operator/(float s)
		{
			return float4(x / s, y / s, z / s, w / s);
		}
	};
}