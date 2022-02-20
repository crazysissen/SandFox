#include "core.h"

#include "TextureBindable.h"

SandFox::Bind::TextureBindable::TextureBindable(const TextureBindable & copy)
	:
	Texture(copy),
	m_registerIndex(copy.m_registerIndex)
{
}

SandFox::Bind::TextureBindable::TextureBindable(unsigned int registerIndex)
	:
	Texture(),
	m_registerIndex(registerIndex)
{
	
}

SandFox::Bind::TextureBindable::TextureBindable(std::wstring name/*std::wstring name*/, unsigned int registerIndex)
	:
	Texture(name),
	m_registerIndex(registerIndex)
{
}

void SandFox::Bind::TextureBindable::Bind()
{
	if (GetResourceView() != nullptr)
	{
		EXC_COMINFO(Graphics::Get().GetContext()->PSSetShaderResources(m_registerIndex, 1u, GetResourceView().GetAddressOf()));
	}
}
