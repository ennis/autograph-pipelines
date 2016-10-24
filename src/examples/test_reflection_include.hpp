#pragma once

namespace common
{
	constexpr float pi = 3.14159f;
	const float three = 3.0f;

	inline float sin(float a)
	{
		return a;
	}

	inline float cos(float a)
	{
		return a;
	}

	float tan(float);

	struct ExampleStruct
	{
		int i;
		int j;
	};

	struct ExampleStruct2
	{
		int i;
		int j;
		double d;
	};
}