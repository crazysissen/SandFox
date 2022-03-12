#pragma once

#include "SandFoxCore.h"

#include <string>

namespace SandFox
{

	class FOX_API Texture
	{
	public:
		Texture();
		Texture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv);
		Texture(const Texture& copy);
		Texture(const std::wstring& name);
		virtual ~Texture();

		void Load(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv);
		virtual void Load(const std::wstring& name, bool immutable = true, D3D11_BIND_FLAG bindFlags = D3D11_BIND_SHADER_RESOURCE);
		virtual void Load(unsigned char* data, int width, int height, bool immutable = true, int stride = 4, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG bindFlags = D3D11_BIND_SHADER_RESOURCE);

		ComPtr<ID3D11Texture2D> GetTexture();
		ComPtr<ID3D11ShaderResourceView> GetResourceView();

	private:
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11ShaderResourceView> m_resourceView;
	};

	class FOX_API RenderTexture : public Texture
	{
	public:
		RenderTexture();
		RenderTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv);
		RenderTexture(const cs::ColorA& color, int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG additionalBindFlags = D3D11_BIND_SHADER_RESOURCE);
		RenderTexture(const RenderTexture& copy);
		RenderTexture(const Texture& texture);
		virtual ~RenderTexture();

		void Load(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv);
		virtual void Load(const std::wstring& name, bool immutable = false, D3D11_BIND_FLAG additionalBindFlags = D3D11_BIND_SHADER_RESOURCE) override;
		virtual void Load(unsigned char* data, int width, int height, bool immutable = false, int stride = 4, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG additionalBindFlags = D3D11_BIND_SHADER_RESOURCE) override;

		virtual void Load(const cs::ColorA& color, int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG additionalBindFlags = D3D11_BIND_SHADER_RESOURCE);
		virtual void Clear(const cs::ColorA& color);

		void CreateRenderTarget(DXGI_FORMAT format);

		ComPtr<ID3D11RenderTargetView> GetRenderTarget();


	private:
		ComPtr<ID3D11RenderTargetView> m_renderTarget;
	};

	class FOX_API UAVTexture
	{
	public:
		UAVTexture();
		UAVTexture(Texture* texture);

		void Load(Texture* texture);

		ComPtr<ID3D11UnorderedAccessView> GetUAV();

	private:
		Texture* m_texture;
		ComPtr<ID3D11UnorderedAccessView> m_uav;
	};

}