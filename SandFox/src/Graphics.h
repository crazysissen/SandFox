#pragma once

#include "IBindable.h"
#include "SandFoxCore.h"
#include "Camera.h"
#include "Texture.h"
#include "SamplerState.h"
#include "ComputeShader.h"
#include "Window.h"

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
		static constexpr bool c_usePPFX = false;
		static constexpr float c_nearClip = 0.05f;
		static constexpr float c_farClip = 1000.0f;
		static constexpr int c_maxRenderTargets = 7;
		static constexpr int c_maxLights = 16;

		static constexpr uint c_registerSceneInfo = 10;
		static constexpr uint c_registerClientInfo = 11;

	public:
		struct Light
		{
			Vec3 position;	// Relevant for point and spot lights
			LightType type;

			Vec3 direction;			// Relevant for directional and spot lights
			float spreadDotLimit;	// Relevant for spot lights

			Vec3 color;
			float intensity;

			static FOX_API Light Directional(const cs::Vec3& direction, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
			static FOX_API Light Point(const cs::Vec3& position, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
			static FOX_API Light Spot(const cs::Vec3& position, const cs::Vec3& direction, float spread = 1.0f, float intensity = 10.0f, const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));
		};

		struct SceneInfo
		{
			Vec3 cameraPos;
			PAD(1, 0);

			Vec3 ambient;
			int lightCount;

			Light lights[c_maxLights];
		};



	public:
		Graphics();
		~Graphics();

		// Initialization/deinitialization logic
		void Init(Window* window, std::wstring shaderDir, GraphicsTechnique technique = GraphicsTechniqueImmediate);
		void DeInit();

		void InitImgui();

		void SetLights(Light* lights, int count);
		void SetLightAmbient(const cs::Color& color, float intensity);

		// Finalize drawing to back buffer and flip
		void FrameBegin(const cs::Color& color);
		void FrameComposite();
		void FrameFinalize();

		void DrawGraphicsImgui();

		void ChangeDepthStencil(bool enable, bool write);

		void PostProcess();

		float GetAspectRatio();
		Window* GetWindow();

		void InitCamera(cs::Vec3 pos, cs::Vec3 rot, float fov);
		void UpdateCamera();
		std::shared_ptr<Camera> GetCamera();
		const dx::XMMATRIX& GetCameraMatrix();

		ComPtr<ID3D11Device>& GetDevice();
		ComPtr<ID3D11DeviceContext>& GetContext();

		std::wstring ShaderPath(std::wstring shaderName);

	public:
		static Graphics& Get();

	private:
		struct ClientInfo
		{
			uint screenWidth;
			uint screenHeight;
			float screenWidthI;
			float screenHeightI;
		};

		static Graphics* s_graphics;

		bool m_initialized;
		bool m_imgui;
		bool m_frameComposited;

		int m_displayedBuffer;

		// Window information
		Window* m_window;

		// Device and associate objects
		ComPtr<ID3D11Device>		m_device;
		ComPtr<IDXGISwapChain>		m_swapChain;
		ComPtr<ID3D11DeviceContext> m_context;

		// Depth stencil
		ComPtr<ID3D11Texture2D>			m_depthStencilTexture;
		ComPtr<ID3D11DepthStencilView>	m_depthStencilView;

		// Technique
		GraphicsTechnique m_technique;

		// Back buffer(s)
		RenderTexture*	m_backBuffers;
		int				m_backBufferCount;
		UAVTexture		m_backBufferUAV;
		ComputeShader   m_lightingPass;

		Bind::SamplerState	m_deferredSamplerState;
		IBindable*			m_deferredClientInfo;
		IBindable*			m_sceneInfoBuffer;
		SceneInfo			m_sceneInfo;

		wrl::ComPtr<ID3D11Debug> m_debug;

		std::shared_ptr<Camera> m_camera;
		dx::XMMATRIX			m_cameraMatrix;
		float					m_aspectRatio;

		std::wstring m_shaderDir;
	};

}