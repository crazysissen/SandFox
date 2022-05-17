#include "pch.h"

#include "PrimitiveTopology.h"
#include "Graphics.h"
#include "BindHandler.h"

SandFox::Bind::PrimitiveTopology::PrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	:
	m_topology(topology)
{
}

void SandFox::Bind::PrimitiveTopology::Load(D3D_PRIMITIVE_TOPOLOGY topology)
{
	m_topology = topology;
}

void SandFox::Bind::PrimitiveTopology::Bind(BindStage stage)
{
	if (BindHandler::BindPT(this))
	{
		EXC_COMINFO(Graphics::Get().GetContext()->IASetPrimitiveTopology(m_topology));
	}
}

SandFox::BindType SandFox::Bind::PrimitiveTopology::Type()
{
	return BindTypePipeline;
}
