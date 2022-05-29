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

		class FOX_API STConstBuffer : public BindableResource
		{
		public:
			STConstBuffer(Transform& transform);
			STConstBuffer(DrawableBase& drawable);
			virtual ~STConstBuffer();

			void Bind(BindStage stage = BindStageVS) override;
			BindType Type() override;

		private:
			ConstBuffer* m_constBuffer;
			Transform& m_transform;
		};

	}
}