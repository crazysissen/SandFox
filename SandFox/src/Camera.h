#pragma once

#include "SandFoxCore.h"
#include <DirectXMath.h>

namespace SandFox
{
	class FOX_API Camera
	{
	public:
		Camera(
			cs::Vec3 position = { 0, 0, 0 },
			cs::Vec3 rotation = { 0, 0, 0 },
			float fov = cs::c_pi * 0.5f,
			float nearClip = FOX_C_NEAR_CLIP_DEFAULT,
			float farClip = FOX_C_FAR_CLIP_DEFAULT,
			float aspectRatio = 16.0f / 9.0f);

		void Load(
			cs::Vec3 position = { 0, 0, 0 },
			cs::Vec3 rotation = { 0, 0, 0 },
			float fov = cs::c_pi * 0.5f,
			float nearClip = FOX_C_NEAR_CLIP_DEFAULT,
			float farClip = FOX_C_FAR_CLIP_DEFAULT,
			float aspectRatio = 16.0f / 9.0f);

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