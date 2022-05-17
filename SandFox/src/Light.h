#pragma once

namespace SandFox
{

	enum LightType
	{
		LightTypeDirectional,
		LightTypePoint,
		LightTypeSpot,
	};

	struct Light
	{
		Vec3 position;	// Relevant for point and spot lights
		LightType type;

		Vec3 direction;			// Relevant for directional and spot lights
		float spreadDotLimit;	// Relevant for spot lights

		Vec3 color;
		float intensity;

		static FOX_API Light Directional(const cs::Vec3& direction, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
		static FOX_API Light Point(const cs::Vec3& position, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
		static FOX_API Light Spot(const cs::Vec3& position, const cs::Vec3& direction, float spread = 1.0f, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
	};

}