#include "pch.h"

#include "PrimitiveTopology.h"
#include "Graphics.h"

SandFox::Bind::PrimitiveTopology::PrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	:
	m_topology(topology)
{
}

void SandFox::Bind::PrimitiveTopology::Load(D3D_PRIMITIVE_TOPOLOGY topology)
{
	m_topology = topology;
}

void SandFox::Bind::PrimitiveTopology::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetPrimitiveTopology(m_topology));
}
