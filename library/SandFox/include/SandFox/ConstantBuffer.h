#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"
#include "GraphicsEnums.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API ConstBuffer : public BindableResource
		{
		public:
			ConstBuffer();
			ConstBuffer(RegCBV reg, const void* constants, unsigned int size, bool immutable = false);
			virtual ~ConstBuffer();

			void Load(RegCBV reg, const void* constants, unsigned int size, bool immutable = false);
			void Write(const void* constants);

			void Bind(BindStage stage) override;
			BindType Type() override;

			void Unbind(BindStage stage);

		protected:
			ComPtr<ID3D11Buffer> m_constantBuffer;
			unsigned int m_size;
		};

	}
}