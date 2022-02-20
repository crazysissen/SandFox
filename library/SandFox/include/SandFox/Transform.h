#pragma once

#include "SandFoxCore.h"

namespace SandFox
{

	class FOX_API Transform
	{
	public:
		Transform(cs::Vec3 position = { 0, 0, 0 }, cs::Vec3 rotation = { 0, 0, 0 }, cs::Vec3 scale = { 1, 1, 1 });

		cs::Vec3 GetPosition() const;
		cs::Vec3 GetRotation() const;
		cs::Vec3 GetScale() const;

		void SetPosition(cs::Vec3 position);
		void SetRotation(cs::Vec3 rotation);
		void SetScale(cs::Vec3 scale);

		cs::Vec3 ChangePosition(cs::Vec3 change);
		cs::Vec3 ChangeRotation(cs::Vec3 change);
		cs::Vec3 ChangeScale(cs::Vec3 change);

		void UpdateMatrix();

		dx::XMMATRIX GetMatrix();
		dx::XMMATRIX GetMatrixSaved() const;

	private:
		cs::Vec3 m_position;
		cs::Vec3 m_rotation;
		cs::Vec3 m_scale;

		dx::XMMATRIX m_matrix;
		bool m_updateMatrix;
	};

}
