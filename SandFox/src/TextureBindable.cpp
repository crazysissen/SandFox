#include "pch.h"

#include "TextureBindable.h"

SandFox::Bind::TextureBindable::TextureBindable(const TextureBindable & copy)
	:
	m_texture(copy.m_texture),
	m_registerIndex(copy.m_registerIndex)
{
}

SandFox::Bind::TextureBindable::TextureBindable(const Texture& texture, unsigned int registerIndex)
	:
	m_texture(texture),
	m_registerIndex(registerIndex)
{
}

SandFox::Bind::TextureBindable::TextureBindable(unsigned int registerIndex)
	:
	m_texture(),
	m_registerIndex(registerIndex)
{
	
}

SandFox::Bind::TextureBindable::TextureBindable(std::wstring name/*std::wstring name*/, unsigned int registerIndex)
	:
	m_texture(name),
	m_registerIndex(registerIndex)
{
}

void SandFox::Bind::TextureBindable::Bind()
{
	if (m_texture.GetResourceView() != nullptr)
	{
		EXC_COMINFO(Graphics::Get().GetContext()->PSSetShaderResources(m_registerIndex, 1u, m_texture.GetResourceView().GetAddressOf()));
	}
}
