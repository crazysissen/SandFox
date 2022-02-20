#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API PrimitiveTopology : public IBindable
		{
		public:
			PrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			void Bind() override;

		private:
			D3D_PRIMITIVE_TOPOLOGY m_topology;
		};

	}
}