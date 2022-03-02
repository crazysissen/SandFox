#include "pch.h"

#include <d3dcompiler.h>
#include <dxgidebug.h>

#include "Graphics.h"
#include "Window.h"



SandFox::Graphics* SandFox::Graphics::s_graphics = nullptr;



SandFox::Graphics::Graphics()
	:
	m_initialized(false),

	m_device(nullptr),
	m_swapChain(nullptr),
	m_context(nullptr),

	m_backBuffer(nullptr),
	m_backBufferSRV(nullptr),
	m_backBufferRTV(nullptr),

	m_depthStencilTexture(nullptr),
	m_depthStencilView(nullptr),

	m_debug(nullptr),

	m_camera(nullptr),
	m_cameraMatrix(),
	m_aspectRatio(),

	m_shaderDir(L"")
{
	s_graphics = this;
}

SandFox::Graphics::~Graphics()
{
	s_graphics = nullptr;
}

void SandFox::Graphics::Init()
{
	if (m_initialized)
	{
		std::cout << "Graphics re-initialization." << std::endl;
		return;
	}

	m_initialized = true;





	// --- Initialize COM debug logger

	cs::dxgiInfo::init();





	uint sampleCount = 1u, sampleQuality = 0;

	// --- Device, swap chain, rendering context, and front/back buffers

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = Window::GetW();
	scd.BufferDesc.Height = Window::GetH();
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = sampleCount;
	scd.SampleDesc.Quality = sampleQuality;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = Window::GetHwnd() /*(HWND)67676*/;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	uint deviceFlags = 0;

#ifndef RELEASE
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	
	EXC_COMCHECKINFO(D3D11CreateDeviceAndSwapChain(
		nullptr, // Let system pick graphics card/interface
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlags,
		featureLevels,
		4,
		D3D11_SDK_VERSION,
		&scd,
		&m_swapChain,
		&m_device,
		nullptr,
		&m_context
	));

	m_backBuffer = nullptr;
	EXC_COMCHECKINFO(m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), &m_backBuffer));
	EXC_COMCHECKINFO(m_device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &m_backBufferRTV));



	//// define function signature of DXGIGetDebugInterface
	//typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	//// load the dll that contains the function DXGIGetDebugInterface
	//const auto hModDxgiDebug = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

	//// get address of DXGIGetDebugInterface in dll
	//const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
	//	reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));

	//DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &debug);

#ifdef _SAFE
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
	D3D11_TEXTURE2D_DESC dstDesc = {};
	dstDesc.Width = Window::GetW();
	dstDesc.Height = Window::GetH();
	dstDesc.MipLevels = 1u;
	dstDesc.ArraySize = 1u;
	dstDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dstDesc.SampleDesc.Count = sampleCount;
	dstDesc.SampleDesc.Quality = sampleQuality;
	dstDesc.Usage = D3D11_USAGE_DEFAULT;
	dstDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	//ComPtr<ID3D11Texture2D> m_depthStencilTexture;
	EXC_COMCHECKINFO(m_device->CreateTexture2D(&dstDesc, nullptr, &m_depthStencilTexture));





	// --- Depth stencil view

	// Create
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Texture2D.MipSlice = 0u;

	EXC_COMCHECKINFO(m_device->CreateDepthStencilView(m_depthStencilTexture.Get(), &dsvDesc, &m_depthStencilView));





	// --- Bind render target

	if (c_usePPFX)
	{
		//EXC_COMINFO(m_context->OMSetRenderTargets(1u, pOffscreenRTV/*m_backBufferRTV*/.GetAddressOf(), g_dsv.Get()));
	}
	else
	{
		EXC_COMINFO(m_context->OMSetRenderTargets(1u, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get()));
	}





	// --- Configure viewport

	D3D11_VIEWPORT vp;
	vp.Width = (float)Window::GetW();
	vp.Height = (float)Window::GetH();
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	EXC_COMINFO(m_context->RSSetViewports(1u, &vp));

	// and aspect ration in conjunction
	m_aspectRatio = (float)vp.Width / (float)vp.Height;






	// --- Rasterizer

	D3D11_RASTERIZER_DESC rsd;
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





	// --- Imgui

	//ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());





	// --- Offscreen render texture

	// Create
	/*D3D11_TEXTURE2D_DESC ortDesc = {};
	m_backBuffer->GetDesc(&ortDesc);
	ortDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	ortDesc.Width = window::getW();
	ortDesc.Height = window::getH();
	ortDesc.ArraySize = 1u;
	ortDesc.MipLevels = 1u;
	ortDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	ortDesc.Usage = D3D11_USAGE_DEFAULT;
	ortDesc.CPUAccessFlags = 0;
	ortDesc.SampleDesc.Count = sampleCount;
	ortDesc.SampleDesc.Quality = sampleQuality;
	ortDesc.MiscFlags = 0u;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = ortDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	rtvDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = ortDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	EXC_COMCHECKINFO(m_device->CreateTexture2D(&ortDesc, nullptr, pOffscreenTexture.GetAddressOf()));
	EXC_COMCHECKINFO(m_device->CreateShaderResourceView(pOffscreenTexture.Get(), &srvDesc, pOffscreenSRV.ReleaseAndGetAddressOf()));
	EXC_COMCHECKINFO(m_device->CreateRenderTargetView(pOffscreenTexture.Get(), &rtvDesc, pOffscreenRTV.ReleaseAndGetAddressOf()));

	EXC_COMCHECKINFO(m_device->CreateTexture2D(&ortDesc, nullptr, pOffscreenTexture2.GetAddressOf()));
	EXC_COMCHECKINFO(m_device->CreateShaderResourceView(pOffscreenTexture2.Get(), &srvDesc, pOffscreenSRV2.ReleaseAndGetAddressOf()));
	EXC_COMCHECKINFO(m_device->CreateRenderTargetView(pOffscreenTexture2.Get(), &rtvDesc, pOffscreenRTV2.ReleaseAndGetAddressOf()));

	EXC_COMCHECKINFO(m_device->CreateTexture2D(&ortDesc, nullptr, pOffscreenTexture3.GetAddressOf()));
	EXC_COMCHECKINFO(m_device->CreateShaderResourceView(pOffscreenTexture3.Get(), &srvDesc, pOffscreenSRV3.ReleaseAndGetAddressOf()));
	EXC_COMCHECKINFO(m_device->CreateRenderTargetView(pOffscreenTexture3.Get(), &rtvDesc, pOffscreenRTV3.ReleaseAndGetAddressOf()));*/





	// --- Post Processing Effects

	//pBasicPostProcess = new DirectX::BasicPostProcess(m_device.Get());
	//pDualPostProcess = new DirectX::DualPostProcess(m_device.Get());

	//pPpfx = new PPFX(L"PSPPFXSample");
}

void SandFox::Graphics::DeInit()
{
	// --- Imgui

	//ImGui_ImplDX11_Shutdown();
}

void SandFox::Graphics::FrameBegin(const cs::Color& color)
{
	const float colorArray[] = { color.r, color.g, color.b, 1.0f };

	m_context->ClearRenderTargetView(m_backBufferRTV.Get(), colorArray);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

	if (c_usePPFX)
	{
		//EXC_COMINFO(m_context->ClearRenderTargetView(pOffscreenRTV.Get(), colorArray));
		//EXC_COMINFO(m_context->ClearRenderTargetView(pOffscreenRTV2.Get(), colorArray));
		////EXC_COMINFO( m_context->ClearRenderTargetView(pOffscreenRTV3.Get(), colorArray) );

		//ID3D11ShaderResourceView* const pSRV[5] = { NULL, NULL, NULL, NULL, NULL };
		//m_context->PSSetShaderResources(0, 5, pSRV);


		//EXC_COMINFO(m_context->OMSetRenderTargets(1u, pOffscreenRTV.GetAddressOf(), g_dsv.Get()));

		//return;
	}

	EXC_COMINFO(m_context->OMSetRenderTargets(1u, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get()));
}

void SandFox::Graphics::FrameFinalize()
{
	m_swapChain->Present(1u, 0);
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

float SandFox::Graphics::GetAspectRatio()
{
	return m_aspectRatio;
}

void SandFox::Graphics::SetShaderDirectory(std::wstring shaderDir)
{
	m_shaderDir = shaderDir + L'\\';
}

void SandFox::Graphics::InitCamera(Vec3 pos, Vec3 rot, float fov)
{
	m_camera = std::make_shared<Camera>(pos, rot, fov, c_nearClip, c_farClip);
}

std::shared_ptr<SandFox::Camera> SandFox::Graphics::GetCamera()
{
	if (m_camera == nullptr)
	{
		EXC("Could not retrieve default camera.");
	}

	return m_camera;
}

void SandFox::Graphics::UpdateCameraMatrix()
{
	m_cameraMatrix = m_camera->GetMatrix();
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

