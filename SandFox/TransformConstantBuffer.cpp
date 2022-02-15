#include "core.h"

#include "TransformConstantBuffer.h"

ibind::TConstBuffer::TConstBuffer(_Drawable& refParent)
	:
	m_refParent(refParent)
{
	m_constBuffer = std::make_unique<ConstBufferV<dx::XMMATRIX>>(dx::XMMatrixIdentity());
}

void ibind::TConstBuffer::Bind()
{
	DirectX::XMMATRIX transformationMatrix = m_refParent.GetTransformationMatrix();
	DirectX::XMMATRIX viewMatrix = Graphics::Get().GetCameraMatrix();

	m_constBuffer->Update
	(
		dx::XMMatrixTranspose // <- flips matrix cpu-side to make gpu calculations more efficient
		(
			transformationMatrix *
			viewMatrix
		)
	);

	m_constBuffer->Bind();
}
