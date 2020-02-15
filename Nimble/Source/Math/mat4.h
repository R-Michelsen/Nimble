#pragma once

#include "float4.h"

namespace Nimble
{
	struct mat4
	{
		union
		{
			float m[4][4];
		};

		// The default constructor of a mat4 is the identity matrix
		mat4() 
		{
			m[0][0] = 1; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
			m[0][1] = 0; m[1][1] = 1; m[2][1] = 0; m[3][1] = 0;
			m[0][2] = 0; m[1][2] = 0; m[2][2] = 1; m[3][2] = 0;
			m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
		}

		mat4(float m00, float m01, float m02, float m03,
			 float m10, float m11, float m12, float m13,
			 float m20, float m21, float m22, float m23,
			 float m30, float m31, float m32, float m33)
		{
			m[0][0] = m00; m[1][0] = m01; m[2][0] = m02; m[3][0] = m03;
			m[0][1] = m10; m[1][1] = m11; m[2][1] = m12; m[3][1] = m13;
			m[0][2] = m20; m[1][2] = m21; m[2][2] = m22; m[3][2] = m23;
			m[0][3] = m30; m[1][3] = m31; m[2][3] = m32; m[3][3] = m33;
		}

		// Constructs a 4x4 matrix with a float4 for each column
		mat4(const float4& a, const float4& b, const float4& c, const float4& d)
		{
			m[0][0] = a.x; m[1][0] = b.x; m[2][0] = c.x; m[3][0] = d.x;
			m[0][1] = a.y; m[1][1] = b.y; m[2][1] = c.y; m[3][1] = d.y;
			m[0][2] = a.z; m[1][2] = b.z; m[2][2] = c.z; m[3][2] = d.z;
			m[0][3] = a.w; m[1][3] = b.w; m[2][3] = c.w; m[3][3] = d.w;
		}

		inline mat4 operator*(const mat4& B)
		{
			const mat4& A = *this;
			return mat4(
				A(0, 0) * B(0, 0) + A(0, 1) * B(1, 0) + A(0, 2) * B(2, 0) + A(0, 3) * B(3, 0),
				A(0, 0) * B(0, 1) + A(0, 1) * B(1, 1) + A(0, 2) * B(2, 1) + A(0, 3) * B(3, 1),
				A(0, 0) * B(0, 2) + A(0, 1) * B(1, 2) + A(0, 2) * B(2, 2) + A(0, 3) * B(3, 2),
				A(0, 0) * B(0, 3) + A(0, 1) * B(1, 3) + A(0, 2) * B(2, 3) + A(0, 3) * B(3, 3),

				A(1, 0) * B(0, 0) + A(1, 1) * B(1, 0) + A(1, 2) * B(2, 0) + A(1, 3) * B(3, 0),
				A(1, 0) * B(0, 1) + A(1, 1) * B(1, 1) + A(1, 2) * B(2, 1) + A(1, 3) * B(3, 1),
				A(1, 0) * B(0, 2) + A(1, 1) * B(1, 2) + A(1, 2) * B(2, 2) + A(1, 3) * B(3, 2),
				A(1, 0) * B(0, 3) + A(1, 1) * B(1, 3) + A(1, 2) * B(2, 3) + A(1, 3) * B(3, 3),

				A(2, 0) * B(0, 0) + A(2, 1) * B(1, 0) + A(2, 2) * B(2, 0) + A(2, 3) * B(3, 0),
				A(2, 0) * B(0, 1) + A(2, 1) * B(1, 1) + A(2, 2) * B(2, 1) + A(2, 3) * B(3, 1),
				A(2, 0) * B(0, 2) + A(2, 1) * B(1, 2) + A(2, 2) * B(2, 2) + A(2, 3) * B(3, 2),
				A(2, 0) * B(0, 3) + A(2, 1) * B(1, 3) + A(2, 2) * B(2, 3) + A(2, 3) * B(3, 3),

				A(3, 0) * B(0, 0) + A(3, 1) * B(1, 0) + A(3, 2) * B(2, 0) + A(3, 3) * B(3, 0),
				A(3, 0) * B(0, 1) + A(3, 1) * B(1, 1) + A(3, 2) * B(2, 1) + A(3, 3) * B(3, 1),
				A(3, 0) * B(0, 2) + A(3, 1) * B(1, 2) + A(3, 2) * B(2, 2) + A(3, 3) * B(3, 2),
				A(3, 0) * B(0, 3) + A(3, 1) * B(1, 3) + A(3, 2) * B(2, 3) + A(3, 3) * B(3, 3)
			);
		}

		inline float4 operator*(const float4& v)
		{
			const mat4& A = *this;
			return float4(
				A(0, 0) * v.x + A(0, 1) * v.y + A(0, 2) * v.z + A(0, 3) * v.w,
				A(1, 0) * v.x + A(1, 1) * v.y + A(1, 2) * v.z + A(1, 3) * v.w,
				A(2, 0) * v.x + A(2, 1) * v.y + A(2, 2) * v.z + A(2, 3) * v.w,
				A(3, 0) * v.x + A(3, 1) * v.y + A(3, 2) * v.z + A(3, 3) * v.w
			);
		}

		inline float4& operator[](int i)
		{
			return *reinterpret_cast<float4*>(m[i]);
		}

		inline const float4& operator[](int i) const
		{
			return *reinterpret_cast<const float4*>(m[i]);
		}

		inline float& operator()(int i, int j)
		{
			return m[j][i];
		}

		inline const float& operator()(int i, int j) const
		{
			return m[j][i];
		}

	};
}