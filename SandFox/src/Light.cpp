#include "pch.h"

#include "Light.h"
#include <numeric>



//dx::XMMATRIX SandFox::Light::GetViewDirectional(const cs::Vec3* samplePoints, int count)
//{
//	// Perpendiculars to the direction
//	Vec3 right = (direction ^ Vec3(0, 1, 0)).Normalize();
//	Vec3 up = right ^ direction;
//
//	float minX = std::numeric_limits<float>::max();
//	float maxX = std::numeric_limits<float>::min();
//
//	float minY = std::numeric_limits<float>::max();
//	float maxY = std::numeric_limits<float>::min();
//
//	for (int i = 0; i < count; i++)
//	{
//		float currentX = samplePoints[i] * right;
//		float currentY = samplePoints[i] * up;
//
//		minX = (currentX < minX) ? currentX : minX;
//		maxX = (currentX > maxX) ? currentX : maxX;
//		minY = (currentY < minY) ? currentY : minY;
//		maxY = (currentY > maxY) ? currentY : maxY;
//	}
//
//	dx::XMMATRIX projection = dx::XMMatrixOrthographicLH(maxX - minX, maxY - minY, farClip, nearClip);
//	
//	Vec3 virtualPosition =
//		right * (0.5f * (minX + maxX)) +
//		up	  * (0.5f * (minY + maxY)) -
//		direction * 50.0f;
//	
//	return
//		dx::XMMatrixTranslation(-virtualPosition.x, -virtualPosition.y, -virtualPosition.z) *
//		dx::XMMatrixRotationY(angles.y) *
//		dx::XMMatrixRotationX(angles.x) *
//		dx::XMMatrixRotationZ(angles.z) *
//		projection;
//}

dx::XMMATRIX SandFox::Light::GetViewDirectional(const cs::Vec3& focalPoint, float distance)
{
	dx::XMMATRIX projection = dx::XMMatrixOrthographicLH(directionalWidth, directionalWidth, farClip, nearClip);
	Vec3 virtualPosition = focalPoint - direction * distance;

	return
		dx::XMMatrixTranslation(-virtualPosition.x, -virtualPosition.y, -virtualPosition.z) *
		dx::XMMatrixRotationY(-angles.y) *
		dx::XMMatrixRotationX(-angles.x) *
		dx::XMMatrixRotationZ(-angles.z) *
		projection;
}

dx::XMMATRIX SandFox::Light::GetViewSpot()
{
	dx::XMMATRIX projection = dx::XMMatrixPerspectiveFovLH(fov, 1, farClip, nearClip);;

	return
		dx::XMMatrixTranslation(-position.x, -position.y, -position.z) *
		dx::XMMatrixRotationY(-angles.y) *
		dx::XMMatrixRotationX(-angles.x) *
		dx::XMMatrixRotationZ(-angles.z) *
		projection;
}

SandFox::Light SandFox::Light::Directional(const cs::Vec3& angles, float intensity, const cs::Color& color)
{
	return Light
	{
		{ 0, 0, 0 },
		LightTypeDirectional,
		(cs::Mat::rotation3Y(angles.y) * cs::Mat::rotation3X(angles.x)) * Vec3(0, 0, 1),
		0.0f,
		color,
		intensity,
		angles,
		0,

		// Shadow-specifics, set by LightHandler. Do not alter if shadows are not properly enabled.
		false,
		0,
		0
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
		intensity,
		{ 0, 0, 0 },
		0,

		// Shadow-specifics, set by LightHandler. Do not alter if shadows are not properly enabled.
		false,
		0,
		0
	};
}

SandFox::Light SandFox::Light::Spot(const cs::Vec3& position, const cs::Vec3& angles, float spread, float intensity, const cs::Color& color)
{
	return Light
	{
		position,
		LightTypeSpot,
		(cs::Mat::rotation3Y(angles.y) * cs::Mat::rotation3X(angles.x))* Vec3(0, 0, 1),
		cosf(spread * 0.5f),
		color,
		intensity,
		angles,
		spread,

		// Shadow-specifics, set by LightHandler. Do not alter if shadows are not properly enabled.
		false,
		0,
		0
	};
}