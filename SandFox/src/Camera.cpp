#include "pch.h"

#include "Graphics.h"
#include "Camera.h"

SandFox::Camera::Camera(Vec3 position, Vec3 rotation, float fov, float nearClip, float farClip, float aspectRatio)
	:
	position(position),
	rotation(rotation),
	m_fov(fov),
	m_nearClip(nearClip),
	m_farClip(farClip),
	m_aspectRatio(aspectRatio)
{
	m_projectionMatrix = dx::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
	//m_projectionMatrixInv = dx::XMMatrixInverse(nullptr, m_projectionMatrix);
}
 
void SandFox::Camera::SetFOV(float fov)
{
	m_fov = fov;

	m_projectionMatrix = dx::XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearClip, m_farClip);
	//m_projectionMatrixInv = dx::XMMatrixInverse(nullptr, m_projectionMatrix);
}

void SandFox::Camera::SetClip(float nearClip, float farClip)
{
	m_nearClip = nearClip;
	m_farClip = farClip;

	m_projectionMatrix = dx::XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearClip, m_farClip);
	//m_projectionMatrixInv = dx::XMMatrixInverse(nullptr, m_projectionMatrix);
}

void SandFox::Camera::SetAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;

	m_projectionMatrix = dx::XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearClip, m_farClip);
	//m_projectionMatrixInv = dx::XMMatrixInverse(nullptr, m_projectionMatrix);
}

float SandFox::Camera::GetFOV()
{
	return m_fov;
}

float SandFox::Camera::GetNearClip()
{
	return m_nearClip;
}

float SandFox::Camera::GetFarClip()
{
	return m_farClip;
}

float SandFox::Camera::GetAspectRatio()
{
	return m_aspectRatio;
}

dx::XMMATRIX SandFox::Camera::GetMatrix()
{
	return
		dx::XMMatrixTranslation(-position.x, -position.y, -position.z) *
		dx::XMMatrixRotationY(rotation.y) *
		dx::XMMatrixRotationX(rotation.x) *
		dx::XMMatrixRotationZ(rotation.z) *
		m_projectionMatrix;
		
}
