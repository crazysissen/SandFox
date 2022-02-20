#pragma once

#include "SandFoxCore.h"
#include "Drawable.h"

namespace SandFox
{
	namespace Prim
	{

		class FOX_API TestCube : public Drawable<TestCube>
		{
		public:
			TestCube(cs::Vec3 position, cs::Vec3 rotation, cs::Vec3 scale);
		};

	}
}