#include "pch.h"

#include "Light.h"

SandFox::Light SandFox::Light::Directional(const cs::Vec3& direction, float intensity, const cs::Color& color)
{
	return Light
	{
		{ 0, 0, 0 },
		LightTypeDirectional,
		direction.Normalized(),
		0.0f,
		color,
		intensity
	};
}

SandFox::Light SandFox::Light::Point(const cs::Vec3& position, float intensity, const cs::Color& color)
{
	return Light
	{
		position,
		LightTypePoint,
		{ 0, 0, 0 },
		0.0f,
		color,
		intensity
	};
}

SandFox::Light SandFox::Light::Spot(const cs::Vec3& position, const cs::Vec3& direction, float spread, float intensity, const cs::Color& color)
{
	return Light
	{
		position,
		LightTypeSpot,
		direction.Normalized(),
		cosf(spread * 0.5f),
		color,
		intensity
	};
}