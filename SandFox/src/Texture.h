#pragma once

#include "SandFoxCore.h"

#include <string>

namespace SandFox
{

	class FOX_API Texture
	{
	public:
		Texture();
		Texture(const Texture& copy);
		Texture(std::wstring name /*std::wstring name*/);
		virtual ~Texture();

		void Load(std::wstring name /*std::wstring name*/);
		void Load(unsigned char* data, int width, int height, bool immutable = true, int stride = 4, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

		ComPtr<ID3D11ShaderResourceView>& GetResourceView();

	private:
		ComPtr<ID3D11Resource> m_texture;
		ComPtr<ID3D11ShaderResourceView> m_resourceView;
	};

}