#pragma once

#include "Graphics.h"

class Texture
{
public:
	Texture();
	Texture(const Texture& copy);
	Texture(std::wstring name /*std::wstring name*/);
	virtual ~Texture();

	void Load(std::wstring name /*std::wstring name*/);

	ComPtr<ID3D11ShaderResourceView>& GetResourceView();

private:
	ComPtr<ID3D11Resource> m_texture;
	ComPtr<ID3D11ShaderResourceView> m_resourceView;
};
