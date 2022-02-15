#include "core.h"

#include "Graphics.h"
#include "Camera.h"

Camera::Camera(Vec3 position, Vec3 rotation, float fov, float nearClip, float farClip)
	:
	position(position),
	rotation(rotation),
	fov(fov),
	nearClip(nearClip),
	farClip(farClip)
{
	projectionMatrix = dx::XMMatrixPerspectiveFovLH(fov, Graphics::Get().GetAspectRatio(), nearClip, farClip);
	projectionMatrixInv = dx::XMMatrixInverse(nullptr, projectionMatrix);
}

void Camera::SetPosition(const Vec3 & pos)
{
	position = pos;
}

void Camera::SetRotation(const Vec3 & angles)
{
	rotation = angles;
}

dx::XMMATRIX Camera::GetMatrix()
{
	return
		dx::XMMatrixTranslation(-position.x, -position.y, -position.z) *
		dx::XMMatrixRotationY(rotation.y) *
		dx::XMMatrixRotationX(rotation.x) *
		dx::XMMatrixRotationZ(rotation.z) *
		projectionMatrix;
		
}
