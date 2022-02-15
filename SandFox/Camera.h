#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera(Vec3 position, Vec3 rotation, float fov, float nearClip, float farClip);

	void SetPosition(const Vec3& pos);
	void SetRotation(const Vec3& angles);

	dx::XMMATRIX GetMatrix();

public:
	Vec3 position, rotation;
	float fov, nearClip, farClip;

private:
	dx::XMMATRIX projectionMatrix, projectionMatrixInv;
};