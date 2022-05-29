#include "pch.h"
#include "TextureQuality.h"

constexpr unsigned int SandFox::GetTextureQuality(TextureQuality quality)
{
	constexpr unsigned int res[] =
	{
		256,
		512,
		1024,
		2048,
		4096,
		8192
	};

	return res[quality];
}
