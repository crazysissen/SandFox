#include "pch.h"

#include "TransformConstantBuffer.h"
#include "Graphics.h"
#include "Drawable.h"
#include "BufferStructs.h"

SandFox::Bind::TConstBuffer::TConstBuffer(Transform& transform)
	:
	BindableResource(RegCBVObjectInfo),
	m_transform(transform)
{
	ObjectInfo m =
	{
		dx::XMMatrixIdentity(), dx::XMMatrixIdentity(), { 0, 0, 0 }, 0
	};

	m_constBuffer = new ConstBuffer(RegCBVObjectInfo, &m, sizeof(ObjectInfo));
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
	dx::XMMATRIX worldMatrix = m_transform.GetMatrix();
	dx::XMMATRIX projectionMatrix = Graphics::GetProjection();

	Vec3 diff = m_transform.GetPosition() - Graphics::Get().GetActiveCamera()->position;

	ObjectInfo m =
	{
		dx::XMMatrixTranspose(worldMatrix),
		dx::XMMatrixTranspose(worldMatrix * projectionMatrix),	// Clip matrix
		m_transform.GetPosition(),
		diff.LengthSq()
	};

	m_constBuffer->Write(&m);
	m_constBuffer->Bind(stage);
}

SandFox::BindType SandFox::Bind::TConstBuffer::Type()
{
	return BindTypeConstantBuffer;
}
