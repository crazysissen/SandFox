#pragma once

#include "IRaycastTarget.h"

namespace cs
{

	class Box : public IRaycastTarget
	{
	public:
		Box(Vec3 center, Vec3 xV, Vec3 yV, Vec3 zV);
		Box(Vec3 center, float width, float height, float depth);
		Box(Vec3 center, float width, float height, float depth, float rX, float rY, float rZ);
		
		bool Raycast(const Line3& line, HitInfo& out) const override;
		bool Intersection(const Line3& line, Vec3& out) const;

	private:
		Vec3 m_origin;
		Vec3 m_x;
		Vec3 m_y;
		Vec3 m_z;
		float m_w;
		float m_h;
		float m_d;
	};

}
