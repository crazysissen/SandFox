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

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}



SandFox::Graphics::Graphics()
	:
	m_initialized(false),
	m_frameComposited(false),
	m_displayedBuffer(0),

	m_window(nullptr),

	m_bindHandler(nullptr),

	m_device(nullptr),
	m_swapChain(nullptr),
	m_context(nullptr),

	m_backBuffers(nullptr),
	m_backBufferCount(0),
	m_backBufferUAV(),
	m_lightingPass(),

	m_clientInfoBuffer(nullptr),
	m_cameraInfoBuffer(nullptr),
	m_cameraInfo(),

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

	m_backgroundColor = cs::Color(0.1f, 0.1f, 0.2f);

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
	scd.BufferCount = 3;
	scd.OutputWindow = m_window->GetHwnd() /*(HWND)67676*/;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;
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





	// Handlers setup

	m_bindHandler = new BindHandler();
	m_bindHandler->Init();





	// Deferred rendering setup

	if (technique == GraphicsTechniqueDeferred)
	{
		// Setup additional back buffers

		m_backBufferCount = FOX_C_MAX_RENDER_TARGETS + 1;
		m_backBuffers = new RenderTexture[m_backBufferCount];

		m_backBuffers[1].Load(cs::ColorA(0, 0, 0, 0), m_window->GetW(), m_window->GetH(), DXGI_FORMAT_R32G32B32A32_FLOAT);
		m_backBuffers[2].Load(cs::ColorA(0, 0, 0, 0), m_window->GetW(), m_window->GetH(), DXGI_FORMAT_R32G32B32A32_FLOAT);

		for (int i = 3; i < 6; i++)
			m_backBuffers[i].Load(cs::ColorA(0, 0, 0, 0), m_window->GetW(), m_window->GetH(), DXGI_FORMAT_R8G8B8A8_UNORM);

		for (int i = 6; i < 8; i++)
			m_backBuffers[i].Load(cs::ColorA(0, 0, 0, 0), m_window->GetW(), m_window->GetH(), DXGI_FORMAT_R32_FLOAT);



		// Setup deferred compute shader

		m_lightingPass.Load(ShaderPath(L"D_CSPhong"));
	}
	else
	{
		m_backBufferCount = 1;
		m_backBuffers = new RenderTexture[1];
	}





	// Back buffer 0 and SRVs

	ID3D11Texture2D* backBufferTexture;
	EXC_COMCHECKINFO(m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&backBufferTexture));
	m_backBuffers[0].Load(backBufferTexture, nullptr);
	m_backBuffers[0].CreateRenderTarget(DXGI_FORMAT_B8G8R8A8_UNORM);

	m_backBufferSRVs = new TextureSRV[m_backBufferCount];
	//m_backBufferSRVs[0].Load(&m_backBuffers[i], RegSRV);

	for (int i = 1; i < m_backBufferCount; i++)
	{
		m_backBufferSRVs[i].Load(&m_backBuffers[i], (RegSRV)(RegSRVDefPosition + i - 1));
	}





	// Shader resources

	ClientInfo ci
	{
		(uint)m_window->GetW(),
		(uint)m_window->GetH(),
		1.0f / (m_window->GetW() - 1),
		1.0f / (m_window->GetH() - 1),
		FOX_C_DEPTH_STENCIL_EXPONENT
	};

	m_cameraInfo.cameraPos = { 0, 0, 0 };
	
	m_clientInfoBuffer = new Bind::ConstBuffer(RegCBVClientInfo, &ci, sizeof(ClientInfo), false);
	m_clientInfoBuffer->Bind(BindStagePS);
	m_clientInfoBuffer->Bind(BindStageCS);

	m_cameraInfoBuffer = new Bind::ConstBuffer(RegCBVCameraInfo, &m_cameraInfo, sizeof(CameraInfo), false);
	m_cameraInfoBuffer->Bind(BindStageVS);
	m_cameraInfoBuffer->Bind(BindStagePS);
	m_cameraInfoBuffer->Bind(BindStageCS);

	BindHandler::ApplyPresetSampler(RegSamplerStandard, BindStagePS);
	BindHandler::ApplyPresetSampler(RegSamplerStandard, BindStageCS);

	m_backBufferUAV.Load(&m_backBuffers[0], RegUAVDefault);
	m_copyPass.Load(ShaderPath(L"CSCopyTexture"));





	// Shader systems

	Shader::LoadPresets(m_technique); 





	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));





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
		D3D11_BIND_DEPTH_STENCIL
	);

	m_depthStencilTextureSRV.Load(
		&m_depthStencilTexture,
		RegSRVCopySource,
		false,
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

	m_viewport.Load(
		(float)m_window->GetW(),
		(float)m_window->GetH(),
		{ 0, 0 },
		0.0f,
		1.0f
	);





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
	DrawableBase::ReleaseStatics();

	delete[] m_backBuffers;

	delete m_clientInfoBuffer;
	delete m_cameraInfoBuffer;

	delete m_bindHandler;
}

void SandFox::Graphics::FrameBegin(const cs::Color& color)
{
	UpdateCamera();

	SetProjection(GetCameraMatrix());
	Shader::ShaderOverride(false);

	m_viewport.Apply();

	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u); 

	m_cameraInfoBuffer->Write(&m_cameraInfo);
	m_cameraInfoBuffer->Bind(BindStagePS);
	m_cameraInfoBuffer->Bind(BindStageCS);

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


		ID3D11RenderTargetView* rtvs[FOX_C_MAX_RENDER_TARGETS];
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

		m_backBufferUAV.Bind(BindStageCS);

		// Compose the final image
		if (m_displayedBuffer == 0)
		{
			// Bind the client info 
			m_clientInfoBuffer->Bind(BindStageCS);

			for (int i = 1; i < m_backBufferCount; i++)
			{
				m_backBufferSRVs[i].Bind(BindStageCS);
			}

			// Run the lighting pass
			m_lightingPass.Dispatch(m_window->GetW(), m_window->GetH());

			// Unbind shader resources before next frame
			for (int i = 1; i < m_backBufferCount; i++)
			{
				m_backBufferSRVs[i].Unbind(BindStageCS);
			}

			m_clientInfoBuffer->Unbind(BindStageCS);
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
				target = m_depthStencilTextureSRV.GetSRV().Get();
				ci.sampleExp = FOX_C_DEPTH_STENCIL_EXPONENT;
			}
			else
			{
				target = m_backBufferSRVs[m_displayedBuffer].GetSRV().Get();
			}

			// Bind the client info 
			m_clientInfoBuffer->Write(&ci);
			m_clientInfoBuffer->Bind(BindStageCS);

			m_context->CSSetShaderResources(RegSRVCopySource, 1, &target);
			m_copyPass.Dispatch(m_window->GetW(), m_window->GetH());

			target = nullptr;
			m_context->CSSetShaderResources(RegSRVCopySource, 1, &target);
		}

		m_backBufferUAV.Unbind(BindStageCS);

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
			m_clientInfoBuffer->Bind(BindStageCS);

			// Bind the new render target as a UAV
			m_backBufferUAV.Bind(BindStageCS);

			ID3D11ShaderResourceView* target = m_depthStencilTextureSRV.GetSRV().Get();
			m_context->CSSetShaderResources(RegSRVCopySource, 1, &target);

			m_copyPass.Dispatch(m_window->GetW(), m_window->GetH());

			target = nullptr;
			m_context->CSSetShaderResources(RegSRVCopySource, 1, &target);

			m_backBufferUAV.Unbind(BindStageCS);

			// Set the finalized image as render target.
			m_context->OMSetRenderTargets(1, m_backBuffers[0].GetRenderTarget().GetAddressOf(), m_depthStencilView.Get());
		}
	}
}

void SandFox::Graphics::DrawFrame(DrawQueue* drawQueue)
{
	FrameBegin(m_backgroundColor);
	drawQueue->DrawMain();
	FrameComposite();
	drawQueue->DrawPost();
	PostProcess();
}

void SandFox::Graphics::Present()
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
			m_backBufferSRVs[m_displayedBuffer].GetSRV()->GetDesc(&srvFormat);
		}
	}
	else
	{
		texture = &m_depthStencilTexture;
		texture->GetTexture().Get()->GetDesc(&textureFormat);
		m_depthStencilTextureSRV.GetSRV().Get()->GetDesc(&srvFormat);
		m_depthStencilView->GetDesc(&dsvFormat);
	}

	ImGui::LabelText("TEX Format", c_dxgiFormatCstr[textureFormat.Format]);

	if (srvFormat.ViewDimension != 0)	ImGui::LabelText("SRV Format", c_dxgiFormatCstr[srvFormat.Format]);
	else								ImGui::LabelText("SRV Format", "N/A");

	if (dsvFormat.ViewDimension != 0)	ImGui::LabelText("DSV Format", c_dxgiFormatCstr[dsvFormat.Format]);
	else								ImGui::LabelText("DSV Format", "N/A");

	//ImGui::End();
}

void SandFox::Graphics::SetDepthStencil(bool enable, bool write, D3D11_COMPARISON_FUNC function)
{
	D3D11_DEPTH_STENCIL_DESC dssd = {};
	dssd.DepthEnable = enable;
	dssd.DepthWriteMask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dssd.DepthFunc = function;

	ComPtr<ID3D11DepthStencilState> pDSState;
	EXC_COMCHECKINFO(m_device->CreateDepthStencilState(&dssd, &pDSState));

	// Bind
	EXC_COMINFO(m_context->OMSetDepthStencilState(pDSState.Get(), 1u));
}

void SandFox::Graphics::SetDepthStencilWrite(bool write)
{
	ID3D11DepthStencilState* dss;
	uint ref;
	m_context->OMGetDepthStencilState(&dss, &ref);

	D3D11_DEPTH_STENCIL_DESC dssd = {};
	dss->GetDesc(&dssd);
	
	dssd.DepthWriteMask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

	ComPtr<ID3D11DepthStencilState> newDSS;
	EXC_COMCHECKINFO(m_device->CreateDepthStencilState(&dssd, &newDSS));
	EXC_COMINFO(m_context->OMSetDepthStencilState(newDSS.Get(), 1u));
}

void SandFox::Graphics::SetBackgroundColor(const cs::Color& color)
{
	m_backgroundColor = color;
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
	m_cameraInfoBuffer->Bind(BindStagePS);
	m_cameraInfoBuffer->Bind(BindStageCS);
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
	m_cameraInfo.cameraPos = m_camera->position;

	m_cameraInfoBuffer->Write(&m_cameraInfo);
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



dx::XMMATRIX SandFox::Graphics::s_projection;

const dx::XMMATRIX& SandFox::Graphics::GetProjection()
{
	return s_projection;
}

void SandFox::Graphics::SetProjection(const dx::XMMATRIX& matrix)
{
	s_projection = matrix;
}

void SandFox::Graphics::ClearProjection()
{
	s_projection = dx::XMMatrixIdentity();
}
