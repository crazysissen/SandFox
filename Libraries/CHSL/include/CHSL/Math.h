#pragma once

#include "CHSLTypedef.h"

namespace cs
{
	
	constexpr float c_pi = 3.14159265f;

	int imod(int val, int ceil);
	size_t stmod(size_t val, size_t ceil);

	int floor(float val);
	int ceil(float val);

	float fclamp(float val, float min, float max);
	int iclamp(int val, int min, int max);

	int iwrap(int val, int min, int max);
	float fwrap(float val, float max);
	float fwrap(float val, float min, float max);

	int iabs(int val);

}