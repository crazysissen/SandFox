#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"
#include "ConstantBuffer.h"
#include "Drawable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API STConstBuffer : public IBindable
		{
		public:
			struct MatrixInfo
			{
				dx::XMMATRIX world;
				dx::XMMATRIX projection;
				PAD(16, 0);
			};

		public:
			STConstBuffer(_Drawable& refParent);
			virtual ~STConstBuffer();

			void Bind() override;

		private:
			ConstBufferV<MatrixInfo>* m_constBuffer;
			_Drawable& m_refParent;
		};

	}
}