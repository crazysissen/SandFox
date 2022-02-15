#include "core.h"

#include "TextureBindable.h"

ibind::TextureBindable::TextureBindable(const TextureBindable & copy)
	:
	Texture(copy),
	m_registerIndex(copy.m_registerIndex)
{
}

ibind::TextureBindable::TextureBindable(uint registerIndex)
	:
	Texture(),
	m_registerIndex(registerIndex)
{
	
}

ibind::TextureBindable::TextureBindable(std::wstring name/*std::wstring name*/, uint registerIndex)
	:
	Texture(name),
	m_registerIndex(registerIndex)
{
}

void ibind::TextureBindable::Bind()
{
	if (GetResourceView() != nullptr)
	{
		EXC_COMINFO(Graphics::Get().GetContext()->PSSetShaderResources(m_registerIndex, 1u, GetResourceView().GetAddressOf()));
	}
}
