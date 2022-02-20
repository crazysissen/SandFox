#include "core.h"

#include "Window.h"
#include "Input.h"
#include "Graphics.h"
#include "CubeInterpolated.h"
#include "TexturePlane.h"
#include "Bindables.h"

#include <dxgidebug.h>
//#include <WICTextureLoader.h>

// Loads D3D11 Library for the whole project
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

Graphics* g_graphics;

void Start()
{

}

void Update(float dTime)
{

}

void Draw(float dTime)
{
	g_graphics->FrameBegin(cs::Color(0x000020));
	g_graphics->UpdateCameraMatrix();



	g_graphics->FrameFinalize();
}

void DeInit()
{


	// Drawables
	_Drawable::ReleaseStatics();

	// DXGI debugger
	std::vector<std::string> messages = cs::dxgiInfo::getMessages();
	for (std::string& m : messages)
		std::cout << m << '\n';
	cs::dxgiInfo::deInit();

	// Graphics system
	g_graphics->DeInit();
	delete g_graphics;
}

int WinMainSafe(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	char*		lpCmdLine,
	int			nCmdShow)
{
	window::initClass(hInstance);
	window::initWindow(window::c_width, window::c_height, "Hello Triangle");

#if (_WIN32_WINNT >= 0x0A00)
	wrl::Wrappers::RoInitializeWrapper init(RO_INIT_MULTITHREADED);
	if (FAILED(init))
	{
		EXC("Could not initialize WRL system.");
	}
#else
	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
#endif

	g_graphics = new Graphics();
	g_graphics->Init();
	g_graphics->InitCamera({ 0, 0, -3 }, { 0, 0, 0 }, cs::c_pi * 0.5f);

	Start();

	window::showWindow();



	// Main message pump and game loop

	cs::Timer timer;

	while (true)
	{
		// Exit code optional evaluates to true if it contains a value
		if (const std::optional<int> exitCode = window::processMessages())
		{
			DeInit();
			return *exitCode;
		}

		float dTime = timer.Lap();
		Update(dTime);
		Draw(dTime);
	}
}

int WINAPI WinMain(
    _In_		HINSTANCE	hInstance, 
    _In_opt_	HINSTANCE	hPrevInstance,
    _In_		char*		lpCmdLine, 
    _In_		int			nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);



	try
	{
		WinMainSafe(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
	catch (const cs::Exception& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details", "Unknown Exception", MB_OK | MB_ICONERROR);
	}

    return 0;
}