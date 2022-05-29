#pragma once

#include "IBindable.h"
#include "SandFoxCore.h"
#include "Camera.h"
#include "Texture.h"
#include "SamplerState.h"
#include "ComputeShader.h"
#include "ConstantBuffer.h"
#include "Window.h"
#include "BindHandler.h"
#include "LightHandler.h"
#include "DrawQueue.h"
#include "Viewport.h"
#include "BufferStructs.h"

#include "GraphicsEnums.h"

#include <string>





// SandFox Graphics engine class
// -----------------------------
// 
// Contains the main render-driving functionality within SandFox.
// 
// Deferred rendering buffer layout:
// |
// | [0] Target buffer			- RGBA	4 bytes
// | 
// | PS Outputs:
// |
// | [1] Position buffer		- XYZW	16 bytes
// | [2] Normal buffer			- XYZW	16 bytes
// | 
// | [3-5] Color textures		- RGBA	4 bytes
// | [6-7] Misc linear targets	- X		4 bytes 
// |
// | [8] Sampler state
// |
// | [10] Lights CBuf			- Same for non-deferred setups using the pixel shader
// | [11] Client CBuf
// |
//





namespace SandFox
{

	class FOX_API Graphics sealed
	{
	public:
		Graphics();
		~Graphics();



		// Initialization/deinitialization logic

		void Init(Window* window, std::wstring shaderDir, GraphicsTechnique technique, Debug* debug = nullptr);
		void DeInit();



		// Frame drawing

		void FrameBegin(const cs::Color& color, Camera* cameraOverride = nullptr, Viewport* viewportOverride = nullptr);
		void FrameMain();
		void FrameComposite();
		void PostProcess();

		// Automatic system
		// Calls FrameBegin, FrameComposite, and PostProcess.
		void DrawFrame(DrawQueue* drawQueue);
		void Present();
		void PresentToTexture(ID3D11Texture2D* target, cs::UPoint size, unsigned int arrIndex = 0);

		void DrawGraphicsImgui();
		void SetDepthStencil(bool enable, bool write, D3D11_COMPARISON_FUNC function = D3D11_COMPARISON_LESS);
		void SetDepthStencilWrite(bool write);
		void SetBackgroundColor(const cs::Color& color);
		void SetTesselation(float nearTesselation, float nearDistancefloat, float interpolationFactor);
		void SetTesselation(float tesselationOverride, float interpolationFactor = 0.75f);
		void EnableTesselation(bool enable);



		// Fetch configured resources and graphics singleton

		ComPtr<ID3D11Device>& GetDevice();
		ComPtr<ID3D11DeviceContext>& GetContext();
		
		Window* GetWindow();
		std::wstring ShaderPath(std::wstring shaderName);

		static Graphics& Get();



		// Camera

		void InitCamera(cs::Vec3 pos, cs::Vec3 rot, float fov, float nearClip = FOX_C_NEAR_CLIP_DEFAULT, float farClip = FOX_C_FAR_CLIP_DEFAULT);
		Camera* GetActiveCamera();
		std::shared_ptr<Camera> GetCamera();
		void ApplyCamera(Camera* camera);
		void ApplyViewport(Viewport* viewport, bool useInverse = false);





		// Projection

		static const dx::XMMATRIX& GetProjection();
		static void SetProjection(const dx::XMMATRIX& matrix);
		static void ClearProjection();




		// Private implementation

	private:

	private:
		static Graphics* s_graphics;
		static dx::XMMATRIX s_projection;

		bool m_initialized;
		bool m_frameComposited;

		int m_displayedBuffer;

		// Window information
		Window* m_window;

		// Handlers
		BindHandler* m_bindHandler;

		// Device and associate objects
		ComPtr<ID3D11Device>		m_device;
		ComPtr<IDXGISwapChain>		m_swapChain;
		ComPtr<ID3D11DeviceContext> m_context;

		// Depth stencil
		Texture							m_depthStencilTexture;
		TextureSRV						m_depthStencilTextureSRV;
		ComPtr<ID3D11DepthStencilView>	m_depthStencilView;

		// Technique
		GraphicsTechnique m_technique;
		cs::Color m_backgroundColor;

		// Back buffer(s)
		RenderTexture*	m_backBuffers;
		TextureSRV*		m_backBufferSRVs;
		int				m_backBufferCount;
		TextureUAV		m_backBufferUAV;
		ComputeShader   m_lightingPass;
		ComputeShader	m_copyPass;

		Viewport	m_viewport;

		Viewport*	m_activeViewport;
		Camera*		m_activeCamera;

		Bind::ConstBuffer*	m_clientInfoBuffer;
		Bind::ConstBuffer*	m_cameraInfoBuffer;
		Bind::ConstBuffer*	m_tesselationBuffer;
		ClientInfo			m_clientInfo;
		CameraInfo			m_cameraInfo;
		TesselationInfo		m_tesselationInfo;
		float				m_tesselationOverride;
		float				m_tesselationNearDistance;
		float				m_tesselationInterpolFactor;

		Debug* m_debug;
		//wrl::ComPtr<ID3D11Debug> m_debug;

		std::shared_ptr<Camera> m_camera;
		dx::XMMATRIX			m_cameraMatrix;

		std::wstring m_shaderDir;
	};

}