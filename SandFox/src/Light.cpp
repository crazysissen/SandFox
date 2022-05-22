#include "pch.h"

#include "Light.h"
#include <numeric>

dx::XMMATRIX SandFox::Light::GetViewDirectional(cs::Vec3 samplePoints[], int count, float nearPlane, float farPlane)
{
	// Perpendiculars to the direction
	Vec3 right = (direction ^ Vec3(0, 1, 0)).Normalize();
	Vec3 up = right ^ direction;

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();

	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();

	for (int i = 0; count; i++)
	{
		float currentX = samplePoints[i] * right;
		float currentY = samplePoints[i] * up;

		minX = (currentX < minX) ? currentX : minX;
		maxX = (currentX > maxX) ? currentX : maxX;
		minY = (currentY < minY) ? currentY : minY;
		maxY = (currentY > maxY) ? currentY : maxY;
	}

	dx::XMMATRIX projection = dx::XMMatrixOrthographicLH(maxX - minX, maxY - minY, nearPlane, farPlane);
	
	Vec3 virtualPosition =
		right * (0.5f * (minX + maxX)) +
		up	  * (0.5f * (minY + maxY));
	
	return
		dx::XMMatrixTranslation(-virtualPosition.x, -virtualPosition.y, -virtualPosition.z) *
		dx::XMMatrixRotationY(angles.y) *
		dx::XMMatrixRotationX(angles.x) *
		dx::XMMatrixRotationZ(angles.z) *
		projection;
}

dx::XMMATRIX SandFox::Light::GetViewSpot(float nearPlane, float farPlane)
{
	dx::XMMATRIX projection = dx::XMMatrixPerspectiveFovLH(fov, 1, nearPlane, farPlane);;

	return
		dx::XMMatrixTranslation(-position.x, -position.y, -position.z) *
		dx::XMMatrixRotationY(angles.y) *
		dx::XMMatrixRotationX(angles.x) *
		dx::XMMatrixRotationZ(angles.z) *
		projection;
}

SandFox::Light SandFox::Light::Directional(const cs::Vec3& angles, bool shadows, float intensity, const cs::Color& color)
{
	return Light
	{
		{ 0, 0, 0 },
		LightTypeDirectional,
		cs::Mat::rotation3(angles.x, angles.y, angles.z) * Vec3(0, 0, 1),
		0.0f,
		color,
		intensity,
		angles,
		shadows,
		0
	};
}

SandFox::Light SandFox::Light::Point(const cs::Vec3& position, bool shadows, float intensity, const cs::Color& color)
{
	if (shadows)
	{
		FOX_WARN("Shadow mapped point lights not supported, mapping disabled for this light.");
	}

	return Light
	{
		position,
		LightTypePoint,
		{ 0, 0, 0 },
		0.0f,
		color,
		intensity,
		{ 0, 0, 0 },
		false,
		0
	};
}

SandFox::Light SandFox::Light::Spot(const cs::Vec3& position, const cs::Vec3& angles, float spread, bool shadows, float intensity, const cs::Color& color)
{
	return Light
	{
		position,
		LightTypeSpot,
		cs::Mat::rotation3(angles.x, angles.y, angles.z) * Vec3(0, 0, 1),
		cosf(spread * 0.5f),
		color,
		intensity,
		angles,
		shadows,
		spread
	};
}