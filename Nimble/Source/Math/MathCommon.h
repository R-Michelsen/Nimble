#pragma once

//#include "float2.h"
//#include "float3.h"
//#include "float4.h"
//#include "uint2.h"
//#include "uint3.h"
//#include "mat4.h"

#include <DirectXMath.h>

namespace Nimble
{
	//using uint2 = DirectX::XMUINT2;
	//using float3 = DirectX::XMFLOAT3A;
	//using float4 = DirectX::XMFLOAT4A;
	//using mat4 = DirectX::XMFLOAT4X4A;

	//constexpr float MATH_CONSTANT_PI = 3.14159265359f;

	///* ----------------- */
	///* Utility functions */
	///* ----------------- */

	//inline float DegToRadians(float deg)
	//{
	//	return (deg * MATH_CONSTANT_PI) / 180.0f;
	//}

	//inline float RadToDegrees(float rad)
	//{
	//	return (rad * 180.0f) / MATH_CONSTANT_PI;
	//}

	///* ---------------- */
	///* Vector functions */
	///* ---------------- */
	//inline float Dot(const float2 a, const float2 b)
	//{
	//	return (a.x * b.x) + (a.y * b.y);
	//}

	//inline float Dot(const float3 a, const float3 b)
	//{
	//	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	//}

	//inline float Dot(const float4 a, const float4 b)
	//{
	//	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	//}

	//inline float3 Cross(const float3 a, const float3 b)
	//{
	//	return float3(
	//		a.y * b.z - a.z * b.y,
	//		a.z * b.x - a.x * b.z,
	//		a.x * b.y - a.y * b.x);
	//}

	//inline float Magnitude(const float2 v)
	//{
	//	return sqrtf(v.x * v.x + v.y * v.y);
	//}

	//inline float Magnitude(const float3 v)
	//{
	//	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	//}

	//inline float Magnitude(const float4 v)
	//{
	//	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	//}

	//inline float2 Normalize(float2 v)
	//{
	//	return v / Magnitude(v);
	//}

	//inline float3 Normalize(float3 v)
	//{
	//	return v / Magnitude(v);
	//}

	//inline float4 Normalize(float4 v)
	//{
	//	return v / Magnitude(v);
	//}

	///* ---------------- */
	///* Matrix functions */
	///* ---------------- */
	//inline mat4 MakeFrustumProjection(const float verticalFOV, const float aspectRatio, const float nearPlane, const float farPlane)
	//{
	//	float g = 1.0f / static_cast<float>(tan(verticalFOV * 0.5f));
	//	float k = farPlane / (farPlane - nearPlane);

	//	return mat4(g / aspectRatio, 0.0f,		0.0f,		0.0f,
	//				0.0f,            g,			0.0f,		0.0f,
	//		        0.0f,            0.0f,		k,			-nearPlane * k,
	//				0.0f,			 0.0f,		1.0f,		0.0f);
	//}

	//inline mat4 MakeView(const float3 eye, const float3 center, const float3 up)
	//{
	//	float3 zAxis = Normalize(eye - center);
	//	float3 xAxis = Normalize(Cross(zAxis, up));
	//	float3 yAxis = Cross(zAxis, xAxis);

	//	return mat4(xAxis.x, xAxis.y, xAxis.z, -Dot(xAxis, eye),
	//				yAxis.x, yAxis.y, yAxis.z, -Dot(yAxis, eye),
	//				zAxis.x, zAxis.y, zAxis.z, -Dot(zAxis, eye),
	//				0.0f,	 0.0f,	  0.0f,    1.0f);
	//}
}