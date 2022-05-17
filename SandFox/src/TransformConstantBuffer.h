#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"
#include "ConstantBuffer.h"
#include "Transform.h"

namespace SandFox
{
	class DrawableBase;

	namespace Bind
	{

		class FOX_API TConstBuffer : public BindableResource
		{
		public:
			TConstBuffer(Transform& transform);
			TConstBuffer(DrawableBase& drawable);
			virtual ~TConstBuffer();

			void Bind(BindStage stage = BindStageVS) override;
			BindType Type() override;

		private:
			ConstBuffer* m_constBuffer;
			Transform& m_transform;
		};

	}
}