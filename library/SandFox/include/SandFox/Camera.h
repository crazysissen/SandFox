#pragma once

#include "SandFoxCore.h"
#include <DirectXMath.h>

namespace SandFox
{
	class FOX_API Camera
	{
	public:
		Camera(cs::Vec3 position, cs::Vec3 rotation, float fov, float nearClip, float farClip, float aspectRatio);

		void SetFOV(float fov);
		void SetClip(float nearClip, float farClip);
		void SetAspectRatio(float aspectRatio);

		float GetFOV();
		float GetNearClip();
		float GetFarClip();
		float GetAspectRatio();

		dx::XMMATRIX GetMatrix();

	public:
		cs::Vec3 position, rotation;

	private:
		float m_fov, m_nearClip, m_farClip, m_aspectRatio;
		dx::XMMATRIX m_projectionMatrix/*, m_projectionMatrixInv*/;
	};
}