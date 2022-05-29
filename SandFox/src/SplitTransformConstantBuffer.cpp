#include "pch.h"

#include "SplitTransformConstantBuffer.h"
#include "Graphics.h"
#include "Drawable.h"
#include "BufferStructs.h"

SandFox::Bind::STConstBuffer::STConstBuffer(Transform& transform)
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

SandFox::Bind::STConstBuffer::STConstBuffer(DrawableBase& drawable)
	:
	STConstBuffer(drawable.transform)
{
}

SandFox::Bind::STConstBuffer::~STConstBuffer()
{
	delete m_constBuffer;
}

void SandFox::Bind::STConstBuffer::Bind(BindStage stage)
{
	dx::XMMATRIX worldMatrix = m_transform.GetMatrix();
	dx::XMMATRIX projectionMatrix = Graphics::GetProjection();

	Vec3 diff = m_transform.GetPosition() - Graphics::Get().GetActiveCamera()->position;

	ObjectInfo m =
	{
		dx::XMMatrixTranspose(worldMatrix),
		dx::XMMatrixTranspose(projectionMatrix),
		m_transform.GetPosition(),
		diff.LengthSq()
	};

	m_constBuffer->Write(&m);
	m_constBuffer->Bind(stage);
}

SandFox::BindType SandFox::Bind::STConstBuffer::Type()
{
	return BindTypeConstantBuffer;
}
