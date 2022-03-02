#pragma once

#include "SandFoxCore.h"
#include "Camera.h"

#include <string>

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

		void SetShaderDirectory(std::wstring shaderDir);
		void InitCamera(cs::Vec3 pos, cs::Vec3 rot, float fov);
		void UpdateCameraMatrix();
		std::shared_ptr<Camera> GetCamera();
		const dx::XMMATRIX& GetCameraMatrix();

		ComPtr<ID3D11Device>& GetDevice();
		ComPtr<ID3D11DeviceContext>& GetContext();

		std::wstring ShaderPath(std::wstring shaderName);

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

		std::wstring m_shaderDir;
	};

}