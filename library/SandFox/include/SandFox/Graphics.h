#pragma once

#include "SandFoxCore.h"
#include "Camera.h"

namespace SandFox
{

	class FOX_API Graphics sealed
	{
	public:
		static constexpr bool c_usePPFX = false;
		static constexpr float c_nearClip = 0.05f;
		static constexpr float c_farClip = 1000.0f;

		Graphics();
		~Graphics();

		// Initialization/deinitialization logic
		void Init();
		void DeInit();

		// Finalize drawing to back buffer and flip
		void FrameBegin(const cs::Color& color);
		void FrameFinalize();

		void PostProcess();

		float GetAspectRatio();

		void InitCamera(cs::Vec3 pos, cs::Vec3 rot, float fov);
		void UpdateCameraMatrix();
		std::shared_ptr<Camera> GetCamera();
		const dx::XMMATRIX& GetCameraMatrix();

		ComPtr<ID3D11Device>& GetDevice();
		ComPtr<ID3D11DeviceContext>& GetContext();

	public:
		static Graphics& Get();

	private:
		static Graphics* s_graphics;

		bool m_initialized;

		// Device and associate objects
		ComPtr<ID3D11Device> m_device;
		ComPtr<IDXGISwapChain> m_swapChain;
		ComPtr<ID3D11DeviceContext> m_context;

		// Back buffer
		ComPtr<ID3D11Texture2D> m_backBuffer;
		ComPtr<ID3D11ShaderResourceView> m_backBufferSRV;
		ComPtr<ID3D11RenderTargetView> m_backBufferRTV;

		// Depth stencil
		ComPtr<ID3D11Texture2D> m_depthStencilTexture;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;

		wrl::ComPtr<ID3D11Debug> m_debug;

		std::shared_ptr<Camera> m_camera;
		dx::XMMATRIX m_cameraMatrix;
		float m_aspectRatio;

		// Textures
		//ComPtr<ID3D11Texture2D> pOffscreenTexture;
		//ComPtr<ID3D11ShaderResourceView> pOffscreenSRV;
		//ComPtr<ID3D11RenderTargetView> pOffscreenRTV;
		//
		//ComPtr<ID3D11Texture2D> pOffscreenTexture2;
		//ComPtr<ID3D11ShaderResourceView> pOffscreenSRV2;
		//ComPtr<ID3D11RenderTargetView> pOffscreenRTV2;
		//
		//ComPtr<ID3D11Texture2D> pOffscreenTexture3;
		//ComPtr<ID3D11ShaderResourceView> pOffscreenSRV3;
		//ComPtr<ID3D11RenderTargetView> pOffscreenRTV3;

		// Post-processing
		//PPFX* pPpfx;
	};

}