#pragma once

#include "SandFoxCore.h"

namespace SandFox
{

	enum TextureQuality
	{
		TextureQualityLowest,
		TextureQualityLower,
		TextureQualityDefault,
		TextureQualityHigher,
		TextureQualityHighest,	
		TextureQualityInsane,	// Danger

		TextureQualityCount		// <-- Keep last!
	};

	constexpr unsigned int FOX_API GetTextureQuality(TextureQuality quality);

}