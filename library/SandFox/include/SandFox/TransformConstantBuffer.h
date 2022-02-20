#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"
#include "ConstantBuffer.h"
#include "Drawable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API TConstBuffer : public IBindable
		{
		public:
			TConstBuffer(_Drawable& refParent);

			void Bind() override;

		private:
			std::unique_ptr<ConstBufferV<dx::XMMATRIX>> m_constBuffer;
			_Drawable& m_refParent;
		};

	}
}