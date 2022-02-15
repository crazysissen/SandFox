#include "core.h"

#include "Transform.h"

Transform::Transform(Vec3 position, Vec3 rotation, Vec3 scale)
	:
	m_position(position),
	m_rotation(rotation),
	m_scale(scale),
	m_matrix(),
	m_updateMatrix(true)
{
}

Vec3 Transform::GetPosition() const
{
	return m_position;
}

Vec3 Transform::GetRotation() const
{
	return m_rotation;
}

Vec3 Transform::GetScale() const
{
	return m_scale;
}

void Transform::SetPosition(Vec3 position)
{
	m_position = position;
	m_updateMatrix = true;
}

void Transform::SetRotation(Vec3 rotation)
{
	m_rotation = rotation;
	m_updateMatrix = true;
}

void Transform::SetScale(Vec3 scale)
{
	m_scale = scale;
	m_updateMatrix = true;
}

Vec3 Transform::ChangePosition(Vec3 change)
{
	SetPosition(m_position + change);
	return m_position;
}

Vec3 Transform::ChangeRotation(Vec3 change)
{
	SetRotation(m_rotation + change);
	return m_rotation;
}

Vec3 Transform::ChangeScale(Vec3 change)
{
	SetScale(m_scale + change);
	return m_scale;
}

void Transform::UpdateMatrix()
{
	if (m_updateMatrix)
	{
		m_matrix = dx::XMMatrixScalingFromVector(m_scale.GetXM3());

		if (m_rotation.y != 0) m_matrix *= dx::XMMatrixRotationY(m_rotation.y);
		if (m_rotation.x != 0) m_matrix *= dx::XMMatrixRotationX(m_rotation.x);
		if (m_rotation.z != 0) m_matrix *= dx::XMMatrixRotationZ(m_rotation.z);

		m_matrix *= dx::XMMatrixTranslationFromVector(m_position.GetXM3());

		m_updateMatrix = false;
	}
}

dx::XMMATRIX Transform::GetMatrix()
{
	UpdateMatrix();
	return m_matrix;
}

dx::XMMATRIX Transform::GetMatrixSaved() const
{
	return m_matrix;
}
