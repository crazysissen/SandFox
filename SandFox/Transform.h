#pragma once

class Transform
{
public:
	Transform(Vec3 position = { 0, 0, 0 }, Vec3 rotation = { 0, 0, 0 }, Vec3 scale = { 1, 1, 1 });

	Vec3 GetPosition() const;
	Vec3 GetRotation() const;
	Vec3 GetScale() const;

	void SetPosition(Vec3 position);
	void SetRotation(Vec3 rotation);
	void SetScale(Vec3 scale);

	Vec3 ChangePosition(Vec3 change);
	Vec3 ChangeRotation(Vec3 change);
	Vec3 ChangeScale(Vec3 change);

	void UpdateMatrix();

	dx::XMMATRIX GetMatrix();
	dx::XMMATRIX GetMatrixSaved() const;

private:
	Vec3 m_position;
	Vec3 m_rotation;
	Vec3 m_scale;

	dx::XMMATRIX m_matrix;
	bool m_updateMatrix;
};

