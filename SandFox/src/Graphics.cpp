#include "pch.h"

#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <cstring>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include "Graphics.h"
#include "Window.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "Drawable.h"



SandFox::Graphics* SandFox::Graphics::s_graphics = nullptr;



SandFox::Graphics::Graphics()
	:
	m_initialized(false),
	m_imgui(false),
	m_frameComposited(false),
	m_displayedBuffer(0),

	m_window(nullptr),

	m_device(nullptr),
	m_swapChain(nullptr),
	m_context(nullptr),

	m_backBuffers(nullptr),
	m_backBufferCount(0),
	m_backBufferUAV(),
	m_lightingPass(),

	m_deferredSamplerState(),
	m_clientInfo(nullptr),
	m_sceneInfoBuffer(nullptr),
	m_sceneInfo(),

	m_depthStencilTexture(),
	m_depthStencilView(nullptr),

	m_technique(GraphicsTechniqueImmediate),

	m_debug(nullptr),

	m_camera(nullptr),
	m_cameraMatrix(),

	m_shaderDir(L"")
{
	s_graphics = s_graphics == nullptr ? this : s_graphics;
}

SandFox::Graphics::~Graphics()
{
	s_graphics = nullptr;
}

void SandFox::Graphics::Init(Window* window, std::wstring shaderDir, GraphicsTechnique technique)
{
	if (m_initialized)
	{
		std::cout << "Graphics re-initialization." << std::endl;
		return;
	}

	m_initialized = true;
	m_window = window;
	m_shaderDir = shaderDir + L'\\';

	cs::dxgiInfo::init();





	// --- Device, swap chain, rendering context, and front/back buffers

	uint deviceFlags = 0;

#ifndef RELEASE
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	uint sampleCount = 1u;
	uint sampleQuality = 0;

	int featureLevelCount = 4;
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};





	// --- Scene info

	m_sceneInfo.cameraPos = { 0, 0, 0 };
	m_sceneInfo.ambient = { 0, 0, 0 };
	m_sceneInfo.lightCount = 0;


	


	// Device, swap chain, and context

	m_technique = technique;

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = m_window->GetW();
	scd.BufferDesc.Height = m_window->GetH();
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = sampleCount;
	scd.SampleDesc.Quality = sampleQuality;
	scd.BufferCount = 2;
	scd.OutputWindow = m_window->GetHwnd() /*(HWND)67676*/;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.Flags = 0;

	if (m_technique == GraphicsTechniqueImmediate)
	{

		scd.BufferUsage = DXGI_USAGE_UNORDERED_ACCESS | DXGI_USAGE_RENDER_TARGET_OUTPUT;

		EXC_COMCHECKINFO(D3D11CreateDeviceAndSwapChain(
			nullptr, // Let system pick graphics card/interface
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			deviceFlags,
			featureLevels,
			featureLevelCount,
			D3D11_SDK_VERSION,
			&scd,
			&m_swapChain,
			&m_device,
			nullptr,
			&m_context
		));
		


		m_backBufferCount = 1;
		m_backBuffers = new RenderTexture[1];
		ID3D11Texture2D* backBufferTexture;
		EXC_COMCHECKINFO(m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&backBufferTexture));
		m_backBuffers[0].Load(backBufferTexture, nullptr, nullptr);
		m_backBuffers[0].CreateRenderTarget(DXGI_FORMAT_B8G8R8A8_UNORM);

		m_sceneInfoBuffer = new Bind::ConstBufferP<SceneInfo>(m_sceneInfo, c_registerSceneInfo, false);

		ClientInfo ci
		{
			(uint)m_window->GetW(),
			(uint)m_window->GetH(),
			1.0f / (m_window->GetW() - 1),
			1.0f / (m_window->GetH() - 1),
			c_depthStencilExponent
		};
		m_clientInfo = new Bind::ConstBufferC<ClientInfo>(ci, c_registerClientInfo, false);

	}
	else
	{

		scd.BufferUsage = DXGI_USAGE_UNORDERED_ACCESS | DXGI_USAGE_RENDER_TARGET_OUTPUT; 

		EXC_COMCHECKINFO(D3D11CreateDeviceAndSwapChain(
			nullptr, // Let system pick graphics card/interface
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			deviceFlags,
			featureLevels,
			featureLevelCount,
			D3D11_SDK_VERSION,
			&scd,
			&m_swapChain,
			&m_device,
			nullptr,
			&m_context
		));

		//m_device->CreateDeferredContext(0, &m_context);



		m_backBufferCount = c_maxRenderTargets + 1;
		m_backBuffers = new RenderTexture[m_backBufferCount];
		
		ID3D11Texture2D* bbTexture;
		EXC_COMCHECKINFO(m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&bbTexture));

		m_backBuffers[0].Load(bbTexture, nullptr, nullptr);
		m_backBuffers[0].CreateRenderTarget(DXGI_FORMAT_B8G8R8A8_UNORM);

		m_backBuffers[1].Load(cs::ColorA(0, 0, 0, 0), m_window->GetW(), m_window->GetH(), DXGI_FORMAT_R32G32B32A32_FLOAT);
		m_backBuffers[2].Load(cs::ColorA(0, 0, 0, 0), m_window->GetW(), m_window->GetH(), DXGI_FORMAT_R32G32B32A32_FLOAT);

		for (int i = 3; i < 6; i++)
			m_backBuffers[i].Load(cs::ColorA(0, 0, 0, 0), m_window->GetW(), m_window->GetH(), DXGI_FORMAT_R8G8B8A8_UNORM);

		for (int i = 6; i < 8; i++)
			m_backBuffers[i].Load(cs::ColorA(0, 0, 0, 0), m_window->GetW(), m_window->GetH(), DXGI_FORMAT_R32_FLOAT);

		m_sceneInfoBuffer = new Bind::ConstBufferC<SceneInfo>(m_sceneInfo, c_registerSceneInfo, false);

		m_lightingPass.Load(ShaderPath(L"D_CSPhong"));

		ClientInfo ci
		{
			(uint)m_window->GetW(),
			(uint)m_window->GetH(),
			1.0f / (m_window->GetW() - 1),
			1.0f / (m_window->GetH() - 1),
			0.0f
		};
		m_clientInfo = new Bind::ConstBufferC<ClientInfo>(ci, c_registerClientInfo, false);

	}

	

	m_deferredSamplerState.Load(8, D3D11_FILTER_MIN_MAG_MIP_POINT);

	m_backBufferUAV.Load(&m_backBuffers[0]);
	m_copyPass.Load(ShaderPath(L"CSCopyTexture"));




	// Shader systems

	Shader::LoadPresets(m_technique); 





	//// define function signature of DXGIGetDebugInterface
	//typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	//// load the dll that contains the function DXGIGetDebugInterface
	//const auto hModDxgiDebug = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

	//// get address of DXGIGetDebugInterface in dll
	//const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
	//	reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));

	//DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &debug);

#ifdef inSAFE
	m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)(&m_debug));
	m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
	//debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);





	// --- Depth stencil state

	// Create
	D3D11_DEPTH_STENCIL_DESC dssDesc = {};
	dssDesc.DepthEnable = TRUE;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

	ComPtr<ID3D11DepthStencilState> pDSState;
	EXC_COMCHECKINFO(m_device->CreateDepthStencilState(&dssDesc, &pDSState));

	// Bind
	EXC_COMINFO(m_context->OMSetDepthStencilState(pDSState.Get(), 1u));





	// --- Depth stencil texture

	// Create
	m_depthStencilTexture.Load(
		nullptr, 
		m_window->GetW(), 
		m_window->GetH(), 
		false, 
		4, 
		DXGI_FORMAT_R32_TYPELESS, 
		D3D11_BIND_DEPTH_STENCIL,
		DXGI_FORMAT_R32_FLOAT
	);





	// --- Depth stencil view

	// Create
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Texture2D.MipSlice = 0u;

	EXC_COMCHECKINFO(m_device->CreateDepthStencilView(m_depthStencilTexture.GetTexture().Get(), &dsvDesc, &m_depthStencilView));





	// --- Configure viewport

	D3D11_VIEWPORT vp = {};
	vp.Width = (float)m_window->GetW();
	vp.Height = (float)m_window->GetH();
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	EXC_COMINFO(m_context->RSSetViewports(1u, &vp));





	// --- Rasterizer

	D3D11_RASTERIZER_DESC rsd = {};
	rsd.AntialiasedLineEnable = false; // 
	rsd.CullMode = D3D11_CULL_BACK;
	rsd.DepthBias = 0;
	rsd.DepthBiasClamp = 0.0f;
	rsd.DepthClipEnable = true;
	rsd.FillMode = D3D11_FILL_SOLID;
	rsd.FrontCounterClockwise = false;
	rsd.MultisampleEnable = false; // 
	rsd.ScissorEnable = false;
	rsd.SlopeScaledDepthBias = 0.0f;

	ComPtr<ID3D11RasterizerState> rss;

	EXC_COMCHECKINFO(m_device->CreateRasterizerState(&rsd, &rss));

	EXC_COMINFO(m_context->RSSetState(rss.Get()));





	// --- Blend state

	D3D11_RENDER_TARGET_BLEND_DESC rtbd = {};
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC bd = {};

	uint sampleMask = 0xffffffff;

	if (technique == GraphicsTechniqueImmediate)
	{
		bd.RenderTarget[0] = rtbd;
	}
	else
	{
		bd.RenderTarget[0] = rtbd;

		bd.RenderTarget[2] = rtbd;
		bd.RenderTarget[3] = rtbd;
		bd.RenderTarget[4] = rtbd;
		bd.RenderTarget[5] = rtbd;
	}

	ComPtr<ID3D11BlendState> bss;

	EXC_COMCHECKINFO(m_device->CreateBlendState(&bd, &bss));

	EXC_COMINFO(m_context->OMSetBlendState(bss.Get(), nullptr, sampleMask));

}

void SandFox::Graphics::DeInit()
{
	m_initialized = false;

	Shader::UnloadPresets();
	_Drawable::ReleaseStatics();

	delete[] m_backBuffers;

	delete m_clientInfo;
	delete m_sceneInfoBuffer;
}

void SandFox::Graphics::InitImgui()
{
	// Imgui

	m_imgui = true;
}

void SandFox::Graphics::SetLights(const Light* lights, int count)
{
	if (count >= c_maxLights)
	{
		EXC(string("Cannot load more than [") + std::to_string(c_maxLights) + "] lights. Given: [" + std::to_string(count) + "]");
	}

	std::memcpy(m_sceneInfo.lights, lights, count * sizeof(Light));
	m_sceneInfo.lightCount = count;
}

void SandFox::Graphics::SetLightAmbient(const cs::Color& color, float intensity)
{
	m_sceneInfo.ambient = (Vec3)color * intensity;
}

void SandFox::Graphics::FrameBegin(const cs::Color& color)
{
	UpdateCamera();

	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

	((Bind::ConstBuffer<SceneInfo>*)m_sceneInfoBuffer)->Write(m_sceneInfo);
	m_sceneInfoBuffer->Bind();

	if (m_technique == GraphicsTechniqueImmediate)
	{
		m_backBuffers[0].Clear(color);
		EXC_COMINFO(m_context->OMSetRenderTargets(1u, m_backBuffers[0].GetRenderTarget().GetAddressOf(), m_depthStencilView.Get()));
	}
	else
	{
		m_backBuffers[0].Clear(color);

		m_backBuffers[1].Clear({ 0, 0, 0, 255 });
		m_backBuffers[2].Clear({ 0, 0, 0, 255 });

		m_backBuffers[3].Clear({ 0, 0, 0, 0 });
		m_backBuffers[4].Clear({ 0, 0, 0, 0 });
		m_backBuffers[5].Clear({ 0, 0, 0, 0 });
						
		m_backBuffers[6].Clear({ 0, 0, 0, 255 });
		m_backBuffers[7].Clear({ 0, 0, 0, 255 });


		ID3D11RenderTargetView* rtvs[c_maxRenderTargets];
		for (int i = 0; i < m_backBufferCount - 1; i++)
		{
			rtvs[i] = m_backBuffers[i + 1].GetRenderTarget().Get();
		}

		EXC_COMINFO(m_context->OMSetRenderTargets(m_backBufferCount - 1, rtvs, m_depthStencilView.Get()));
	}

	m_frameComposited = false;
}

void SandFox::Graphics::FrameComposite()
{
	m_frameComposited = true;

	if (m_technique == GraphicsTechniqueDeferred)
	{
		// Clear render targets and reassign them as shader resources for the compute shader
		m_context->OMSetRenderTargets(0, nullptr, nullptr);


		m_deferredSamplerState.Bind();

		// Bind the new render target as a UAV
		m_context->CSSetUnorderedAccessViews(0u, 1u, m_backBufferUAV.GetUAV().GetAddressOf(), nullptr);

		if (m_displayedBuffer == 0)
		{
			// Compose the final image

			// Bind the client info 
			m_clientInfo->Bind();

			for (int i = 1; i < m_backBufferCount; i++)
			{
				EXC_COMINFO(m_context->CSSetShaderResources(i, 1u, m_backBuffers[i].GetResourceView().GetAddressOf()));
			}

			// Run the lighting pass
			m_lightingPass.Dispatch(m_window->GetW(), m_window->GetH());

			// Unbind shader resources before next frame
			ID3D11ShaderResourceView* empty[c_maxRenderTargets] = { nullptr };
			m_context->CSSetShaderResources(1, m_backBufferCount - 1, empty);
			m_context->CSSetSamplers(8, 1, m_deferredSamplerState.GetSamplerState().GetAddressOf());
		}
		else
		{
			ClientInfo ci
			{
				(uint)m_window->GetW(),
				(uint)m_window->GetH(),
				1.0f / (m_window->GetW() - 1),
				1.0f / (m_window->GetH() - 1),
				0.0f
			};

			ID3D11ShaderResourceView* target = nullptr;

			if (m_displayedBuffer >= m_backBufferCount)
			{
				target = m_depthStencilTexture.GetResourceView().Get();
				ci.sampleExp = c_depthStencilExponent;
			}
			else
			{
				target = m_backBuffers[m_displayedBuffer].GetResourceView().Get();
			}

			// Bind the client info 
			((Bind::ConstBufferC<ClientInfo>*)m_clientInfo)->Write(ci);
			m_clientInfo->Bind();

			m_context->CSSetShaderResources(0, 1, &target);
			m_copyPass.Dispatch(m_window->GetW(), m_window->GetH());

			target = nullptr;
			m_context->CSSetShaderResources(0, 1, &target);
		}

		ID3D11UnorderedAccessView* emptyUAV = nullptr;
		m_context->CSSetUnorderedAccessViews(0u, 1u, &emptyUAV, nullptr);

		// Set the finalized image as render target.
		m_context->OMSetRenderTargets(1, m_backBuffers[0].GetRenderTarget().GetAddressOf(), m_depthStencilView.Get());
	}
	else
	{
		// Graphics technique is Immediate

		if (m_displayedBuffer >= m_backBufferCount)
		{
			// Clear render targets and reassign them as shader resources for the compute shader
			m_context->OMSetRenderTargets(0, nullptr, nullptr);

			// Bind the client info 
			m_clientInfo->Bind();
			m_deferredSamplerState.Bind();

			// Bind the new render target as a UAV
			m_context->CSSetUnorderedAccessViews(0u, 1u, m_backBufferUAV.GetUAV().GetAddressOf(), nullptr);

			ID3D11ShaderResourceView* target = m_depthStencilTexture.GetResourceView().Get();
			m_context->CSSetShaderResources(0, 1, &target);

			m_copyPass.Dispatch(m_window->GetW(), m_window->GetH());

			target = nullptr;
			m_context->CSSetShaderResources(0, 1, &target);

			ID3D11UnorderedAccessView* emptyUAV = nullptr;
			m_context->CSSetUnorderedAccessViews(0u, 1u, &emptyUAV, nullptr);

			// Set the finalized image as render target.
			m_context->OMSetRenderTargets(1, m_backBuffers[0].GetRenderTarget().GetAddressOf(), m_depthStencilView.Get());
		}
	}
}

void SandFox::Graphics::FrameFinalize()
{
	if (m_technique == GraphicsTechniqueDeferred && !m_frameComposited)
	{
		FrameComposite();
	}

	EXC_COMCHECKINFO(m_swapChain->Present(0u, 0u));
}

void SandFox::Graphics::DrawGraphicsImgui()
{
	//ImGui::Begin("Graphics");

	if (m_technique == GraphicsTechniqueDeferred)
	{
		cstr combo = "Composited Image\0Position Buffer\0Normal Buffer\0Albedo Color\0- Color\0- Color\0Specular Exponent\0- Linear\0Depth Stencil";
		ImGui::Combo("Display Buffer", &m_displayedBuffer, combo, 9);
	}
	else
	{
		cstr combo = "Render Target\0Depth Stencil";
		ImGui::Combo("Display Buffer", &m_displayedBuffer, combo, 9);
	}

	Texture* texture;
	D3D11_TEXTURE2D_DESC textureFormat = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvFormat = {};
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvFormat = {};
	if (m_displayedBuffer < m_backBufferCount)
	{
		texture = &m_backBuffers[m_displayedBuffer];
		texture->GetTexture().Get()->GetDesc(&textureFormat);

		if (m_displayedBuffer != 0)
		{
			texture->GetResourceView().Get()->GetDesc(&srvFormat);
		}
	}
	else
	{
		texture = &m_depthStencilTexture;
		texture->GetTexture().Get()->GetDesc(&textureFormat);
		texture->GetResourceView().Get()->GetDesc(&srvFormat);
		m_depthStencilView->GetDesc(&dsvFormat);
	}

	ImGui::LabelText("TEX Format", c_dxgiFormatCstr[textureFormat.Format]);

	if (srvFormat.ViewDimension != 0)	ImGui::LabelText("SRV Format", c_dxgiFormatCstr[srvFormat.Format]);
	else								ImGui::LabelText("SRV Format", "N/A");

	if (dsvFormat.ViewDimension != 0)	ImGui::LabelText("DSV Format", c_dxgiFormatCstr[dsvFormat.Format]);
	else								ImGui::LabelText("DSV Format", "N/A");

	//ImGui::End();
}

void SandFox::Graphics::ChangeDepthStencil(bool enable, bool write)
{
	D3D11_DEPTH_STENCIL_DESC dssDesc = {};
	dssDesc.DepthEnable = enable; 
	dssDesc.DepthWriteMask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

	ComPtr<ID3D11DepthStencilState> pDSState;
	EXC_COMCHECKINFO(m_device->CreateDepthStencilState(&dssDesc, &pDSState));

	// Bind
	EXC_COMINFO(m_context->OMSetDepthStencilState(pDSState.Get(), 1u));
}

void SandFox::Graphics::PostProcess()
{
	//if (usePPFX)
	//{
	//	ID3D11ShaderResourceView* const pSRV[5] = { NULL, NULL, NULL, NULL, NULL };
	//	m_context->PSSetShaderResources(0, 5, pSRV);

	//	pPpfx->Begin(m_backBufferRTV).Bind(pOffscreenSRV).Draw();
	//}
}

inline SandFox::Window* SandFox::Graphics::GetWindow()
{
	return m_window;
}

void SandFox::Graphics::InitCamera(Vec3 pos, Vec3 rot, float fov, float nearClip, float farClip)
{
	m_camera = std::make_shared<Camera>(pos, rot, fov, nearClip, farClip, (float)m_window->GetW() / (float)m_window->GetH());
}

std::shared_ptr<SandFox::Camera> SandFox::Graphics::GetCamera()
{
	if (m_camera == nullptr)
	{
		EXC("Could not retrieve default camera.");
	}

	return m_camera;
}

void SandFox::Graphics::UpdateCamera()
{
	m_cameraMatrix = m_camera->GetMatrix();
	m_sceneInfo.cameraPos = m_camera->position;
}

const dx::XMMATRIX& SandFox::Graphics::GetCameraMatrix()
{
	return m_cameraMatrix;
}

ComPtr<ID3D11Device>& SandFox::Graphics::GetDevice()
{
	return m_device;
}

ComPtr<ID3D11DeviceContext>& SandFox::Graphics::GetContext()
{
	return m_context;
}

std::wstring SandFox::Graphics::ShaderPath(std::wstring shaderName)
{
	return m_shaderDir + shaderName;
}

SandFox::Graphics& SandFox::Graphics::Get()
{
	return *s_graphics;
}

SandFox::Graphics::Light SandFox::Graphics::Light::Directional(const cs::Vec3& direction, float intensity, const cs::Color& color)
{
	return Light
	{
		{ 0, 0, 0 },
		LightTypeDirectional,
		direction.Normalized(),
		0.0f,
		color,
		intensity
	};
}

SandFox::Graphics::Light SandFox::Graphics::Light::Point(const cs::Vec3& position, float intensity, const cs::Color& color)
{
	return Light
	{
		position,
		LightTypePoint,
		{ 0, 0, 0 },
		0.0f,
		color,
		intensity
	};
}

SandFox::Graphics::Light SandFox::Graphics::Light::Spot(const cs::Vec3& position, const cs::Vec3& direction, float spread, float intensity, const cs::Color& color)
{
	return Light
	{
		position,
		LightTypeSpot,
		direction.Normalized(),
		cosf(spread * 0.5f),
		color,
		intensity
	};
}
