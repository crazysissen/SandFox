#include "core.h"

#include "SplitTransformConstantBuffer.h"

SandFox::Bind::STConstBuffer::STConstBuffer(_Drawable& refParent)
	:
	m_refParent(refParent)
{
	MatrixInfo m =
	{
		dx::XMMatrixIdentity(), dx::XMMatrixIdentity()
	};

	m_constBuffer = new ConstBufferV<MatrixInfo>(m);
}

SandFox::Bind::STConstBuffer::~STConstBuffer()
{
	delete m_constBuffer;
}

void SandFox::Bind::STConstBuffer::Bind()
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
