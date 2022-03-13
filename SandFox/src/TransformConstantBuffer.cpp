#include "pch.h"

#include "TransformConstantBuffer.h"

SandFox::Bind::TConstBuffer::TConstBuffer(_Drawable& refParent)
	:
	m_refParent(refParent)
{
	m_constBuffer = std::make_unique<ConstBufferV<dx::XMMATRIX>>(dx::XMMatrixIdentity(), 0u, false);
}

void SandFox::Bind::TConstBuffer::Bind()
{
	DirectX::XMMATRIX transformationMatrix = m_refParent.GetTransformationMatrix();
	DirectX::XMMATRIX viewMatrix = Graphics::Get().GetCameraMatrix();

	m_constBuffer->Write
	(
		dx::XMMatrixTranspose // <- flips matrix cpu-side to make gpu calculations more efficient
		(
			transformationMatrix *
			viewMatrix
		)
	);

	m_constBuffer->Bind();
}
