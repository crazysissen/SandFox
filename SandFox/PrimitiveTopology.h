#pragma once

#include "IBindable.h"

namespace ibind
{

	class PrimitiveTopology : public IBindable
	{
	public:
		PrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void Bind() override;

	private:
		D3D_PRIMITIVE_TOPOLOGY m_topology;
	};

}