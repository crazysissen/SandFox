#pragma once

#include "SandFoxCore.h"

namespace SandFox
{

	class FOX_API Viewport
	{
	public:
		Viewport();
		~Viewport();

		void Load(unsigned int width, unsigned int height, Vec2 corner = { 0, 0 }, float minDepth = 0, float maxDepth = 1);
		void Apply();

		cs::UPoint GetDimensions();

	private:
		D3D11_VIEWPORT m_viewport;
		cs::UPoint m_dimensions;
	};

}

