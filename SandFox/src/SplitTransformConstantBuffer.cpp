#include "pch.h"

#include "SplitTransformConstantBuffer.h"
#include "Graphics.h"
#include "BindHandler.h"
#include "Drawable.h"

SandFox::Bind::STConstBuffer::STConstBuffer(Transform& transform)
	:
	BindableResource(RegCBVObjectInfo),
	m_transform(transform)
{
	MatrixInfo m =
	{
		dx::XMMatrixIdentity(), dx::XMMatrixIdentity()
	};

	m_constBuffer = new ConstBuffer(RegCBVObjectInfo, &m, sizeof(MatrixInfo)); 
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
	dx::XMMATRIX projectionMatrix = Graphics::GetProjection();
	dx::XMMATRIX transformationMatrix = m_transform.GetMatrix();

	MatrixInfo m =
	{
		dx::XMMatrixTranspose(transformationMatrix), // <- flips matrix cpu-side to make gpu calculations more efficient
		dx::XMMatrixTranspose(projectionMatrix)
	};

	m_constBuffer->Write(&m);
	m_constBuffer->Bind(stage);
}

SandFox::BindType SandFox::Bind::STConstBuffer::Type()
{
	return BindTypeConstantBuffer;
}
