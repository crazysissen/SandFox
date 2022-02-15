#include "core.h"

#include "SplitTransformConstantBuffer.h"

ibind::STConstBuffer::STConstBuffer(_Drawable& refParent)
	:
	m_refParent(refParent)
{
	MatrixInfo m =
	{
		dx::XMMatrixIdentity(), dx::XMMatrixIdentity()
	};

	m_constBuffer = std::make_unique<ConstBufferV<MatrixInfo>>(m);
}

ibind::STConstBuffer::~STConstBuffer()
{
}

void ibind::STConstBuffer::Bind()
{
	DirectX::XMMATRIX transformationMatrix = m_refParent.GetTransformationMatrix();
	DirectX::XMMATRIX viewMatrix = Graphics::Get().GetCameraMatrix();

	m_constBuffer->Update
	(
		{
			dx::XMMatrixTranspose(transformationMatrix), // <- flips matrix cpu-side to make gpu calculations more efficient
			dx::XMMatrixTranspose(viewMatrix)
		}
	);

	m_constBuffer->Bind();
}
