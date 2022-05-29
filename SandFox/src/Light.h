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

		Vec3 angles;
		float fov;

		float directionalWidth;
		float nearClip;
		float farClip;
		int shadowIndex;

		dx::XMMATRIX projection;

		dx::XMMATRIX GetViewDirectional(const cs::Vec3& focalPoint, float distance);
		dx::XMMATRIX GetViewSpot();

		static FOX_API Light Directional(const cs::Vec3& angles, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
		static FOX_API Light Point(const cs::Vec3& position, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
		static FOX_API Light Spot(const cs::Vec3& position, const cs::Vec3& angles, float spread, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
	};

}