#pragma once

#include "Drawable.h"

namespace prim
{

	class TestCube : public Drawable<TestCube>
	{
	public:
		TestCube(Vec3 position, Vec3 rotation, Vec3 scale);
	};

}