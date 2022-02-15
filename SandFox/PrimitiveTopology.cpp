#include "core.h"

#include "PrimitiveTopology.h"

ibind::PrimitiveTopology::PrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	:
	m_topology(topology)
{
}

void ibind::PrimitiveTopology::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetPrimitiveTopology(m_topology));
}
