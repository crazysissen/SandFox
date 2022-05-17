#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API PrimitiveTopology : public Bindable
		{
		public:
			PrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			void Load(D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;

		private:
			D3D_PRIMITIVE_TOPOLOGY m_topology;
		};

	}
}