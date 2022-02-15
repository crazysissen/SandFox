#pragma once

#include "Drawable.h"

namespace prim
{

	class TexturePlane : public Drawable<TexturePlane>
	{
	public:
		TexturePlane(Transform t, std::wstring textureName);

	private:

	};

}
