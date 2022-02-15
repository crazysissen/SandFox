#pragma once

#include "IBindable.h"
#include "ConstantBuffer.h"
#include "Drawable.h"

namespace ibind
{

	class STConstBuffer : public IBindable
	{
	public:
		struct MatrixInfo
		{
			dx::XMMATRIX world;
			dx::XMMATRIX projection;
			PAD(16);
		};

	public:
		STConstBuffer(_Drawable& refParent);
		virtual ~STConstBuffer();

		void Bind() override;

	private:
		std::unique_ptr<ConstBufferV<MatrixInfo>> m_constBuffer;
		_Drawable& m_refParent;
	};

}