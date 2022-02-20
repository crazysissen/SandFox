#pragma once

#include <DirectXMath.h>

namespace SandFox
{
	class Camera
	{
	public:
		Camera(cs::Vec3 position, cs::Vec3 rotation, float fov, float nearClip, float farClip);

		void SetPosition(const cs::Vec3& pos);
		void SetRotation(const cs::Vec3& angles);

		dx::XMMATRIX GetMatrix();

	public:
		cs::Vec3 position, rotation;
		float fov, nearClip, farClip;

	private:
		dx::XMMATRIX projectionMatrix, projectionMatrixInv;
	};
}