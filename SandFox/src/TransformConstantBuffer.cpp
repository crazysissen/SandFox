#include "pch.h"

#include "TransformConstantBuffer.h"
#include "Graphics.h"
#include "Drawable.h"

SandFox::Bind::TConstBuffer::TConstBuffer(Transform& transform)
	:
	BindableResource(RegCBVObjectInfo),
	m_transform(transform)
{
	dx::XMMATRIX def = dx::XMMatrixIdentity();

	m_constBuffer = new ConstBuffer(RegCBVObjectInfo, &def, sizeof(dx::XMMATRIX));
}

SandFox::Bind::TConstBuffer::TConstBuffer(DrawableBase& drawable)
	:
	TConstBuffer(drawable.transform)
{
}

SandFox::Bind::TConstBuffer::~TConstBuffer()
{
	delete m_constBuffer;
}

void SandFox::Bind::TConstBuffer::Bind(BindStage stage)
{
	dx::XMMATRIX transformationMatrix = m_transform.GetMatrix();
	dx::XMMATRIX cameraMatrix = Graphics::Get().GetCameraMatrix();
	dx::XMMATRIX viewMatrix = dx::XMMatrixTranspose // <- flips matrix cpu-side to make gpu calculations more efficient
	(
		transformationMatrix *
		cameraMatrix
	);

	m_constBuffer->Write(&viewMatrix);
	m_constBuffer->Bind(stage);
}

SandFox::BindType SandFox::Bind::TConstBuffer::Type()
{
	return BindTypeConstantBuffer;
}
