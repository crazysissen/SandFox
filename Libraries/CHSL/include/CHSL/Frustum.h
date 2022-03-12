#pragma once

#include "Vector.h"
#include "Plane.h"
#include "Matrix.h"
#include "Box.h"

namespace cs
{

	struct Frustum
	{
	public:
		Frustum();
		Frustum(const cs::Plane& near, const cs::Plane& far, const cs::Plane& a, const cs::Plane& b, const cs::Plane& c, const cs::Plane& d);
		Frustum(const Frustum&);

		bool Contains(const cs::Vec3& vector);
		bool Contains(const cs::Box& box);

		bool Intersects(const cs::Box& box);
		bool IntersectsFuzzy(const cs::Box& box);
		bool Intersects(const cs::BoxUnaligned& box);
		bool IntersectsFuzzy(const cs::BoxUnaligned& box);

	public:
		// 0: Near (-z)
		// 1: Far (+z)
		// 2-5: Sides (+y, +x, -y, -x)
		cs::Plane sides[6];
	};

	class ViewFrustum : public Frustum
	{
	public:
		ViewFrustum();
		ViewFrustum(const cs::Vec3& position, const cs::Mat3& direction, float near = 0.01f, float far = 10000.0f, float yAngle = 90.0f, float aspectRatio = 16.0f / 9.0f);
		ViewFrustum(const cs::Vec3& position, const cs::Vec3& angles, float near = 0.01f, float far = 10000.0f, float yAngle = 90.0f, float aspectRatio = 16.0f / 9.0f);
		ViewFrustum(const ViewFrustum&);

		void SetPosition(const cs::Vec3& position);
		void SetNearPlane(float near);
		void SetFarPlane(float far);
		void SetViewDirection(const cs::Mat3& direction);
		void SetViewDirection(const cs::Vec3& angles);
		void SetViewAngle(float yAngle, float aspectRatio);

	private:
		void UpdateNearPlane(float near);
		void UpdateFarPlane(float far);
		void UpdateViewDirection(const cs::Mat3& direction);
		void UpdateViewAngle(float yAngle, float aspectRatio);

	private:
		cs::Vec3 m_position;
		cs::Mat3 m_orientation;
		cs::Vec3 m_defaultOrigins[6];
		cs::Vec3 m_defaultSideDirections[4];
	};

}
