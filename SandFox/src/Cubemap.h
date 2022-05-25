#pragma once

#include "SandFoxCore.h"
#include "Drawable.h"
#include "IBindable.h"

namespace SandFox
{

	class FOX_API Cubemap : public BindableResource
	{
	public:
		Cubemap();
		~Cubemap();

		void Load(RegSRV reg, const std::wstring& path, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
		void Load(RegSRV reg, const std::wstring& path, D3D11_USAGE usage, D3D11_BIND_FLAG additionalBindFlags);

		void Bind(BindStage stage = BindStageNone) override;
		BindType Type() override;


	private:
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11ShaderResourceView> m_srv;
	};

	class FOX_API CubemapDrawable : public Drawable<CubemapDrawable>, public IDrawable
	{
	public:
		CubemapDrawable(Transform t, Cubemap* cubemap, bool ownCubemap = false);
		~CubemapDrawable();

		void Draw() override;
		
	private:
		Cubemap* m_cubemap;
		bool m_owner;
	};

}
