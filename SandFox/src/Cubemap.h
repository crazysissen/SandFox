#pragma once

#include "SandFoxCore.h"
#include "Drawable.h"
#include "DrawQueue.h"
#include "IBindable.h"
#include "TextureQuality.h"
#include "Camera.h"
#include "Viewport.h"



// Cubemap format
// 
// 
// Cross shaped image like this: (middle of cross is forward)
//		[+Y]
// [-X] [+Z] [+X] [-Z]
//		[-Y]
// 
// 
// Order of images in Texture2DArray:
// 0:	+X
// 1:	-X
// 2:	+Y
// 3:	-Y
// 4:	+Z
// 5:	-Z
//



namespace SandFox
{

	class FOX_API Cubemap : public BindableResource
	{
	public:
		Cubemap();
		~Cubemap();

		void Load(RegSRV reg, unsigned int faceSideWidth, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, unsigned int additionalBindFlags = 0);
		void Load(RegSRV reg, const std::wstring& path, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
		void Load(RegSRV reg, const std::wstring& path, D3D11_USAGE usage, unsigned int additionalBindFlags);

		void Bind(BindStage stage = BindStageNone) override;
		BindType Type() override;

		const ComPtr<ID3D11Texture2D>& GetTexture();
		const ComPtr<ID3D11ShaderResourceView>& GetSRV();

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

	class FOX_API CubemapDynamic : public Cubemap
	{
	public:
		CubemapDynamic();
		~CubemapDynamic();

		void Load(cs::Vec3 position, TextureQuality quality, RegSRV reg, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
		void Load(cs::Vec3 position, TextureQuality quality, RegSRV reg, D3D11_USAGE usage, unsigned int additionalBindFlags);

		void Draw(DrawQueue* drawQueue);

	private:
		cs::Vec3 m_position;
		Viewport m_viewport;
		Camera m_cameras[6];
	};

}
