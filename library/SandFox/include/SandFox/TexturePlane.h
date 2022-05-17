#pragma once

#include "SandFoxCore.h"
#include "Drawable.h"
#include "Transform.h"

#include <string>

namespace SandFox
{ 
	namespace Prim
	{

		class FOX_API TexturePlane : public Drawable<TexturePlane>, public IDrawable
		{
		public:
			TexturePlane(Transform t, std::wstring textureName, cs::Point tiling = { 1, 1 });

			void Draw() override;
		};

	}
}
