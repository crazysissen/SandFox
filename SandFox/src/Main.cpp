#include "pch.h"

#include "Window.h"
#include "Input.h"
#include "Graphics.h"
#include "CubeInterpolated.h"
#include "TexturePlane.h"
#include "Bindables.h"
#include "Model.h"

#include <dxgidebug.h>
//#include <WICTextureLoader.h>

// Loads D3D11 Library for the whole project
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

void Start()
{

}

void Update(float dTime)
{

}

void Draw(float dTime)
{
	//g_graphics->FrameBegin(cs::Color(0x000020));
	//g_graphics->UpdateCameraMatrix();



	//g_graphics->FrameFinalize();
}

void DeInit()
{


	// Drawables
	//_Drawable::ReleaseStatics();

	//// DXGI debugger
	//std::vector<std::string> messages = cs::dxgiInfo::getMessages();
	//for (std::string& m : messages)
	//	std::cout << m << '\n';
	//cs::dxgiInfo::deInit();

	//// Graphics system
	//g_graphics->DeInit();
	//delete g_graphics;
}

int WinMainSafe(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	char*		lpCmdLine,
	int			nCmdShow)
{
	#if (_WIN32_WINNT >= 0x0A00)
		wrl::Wrappers::RoInitializeWrapper init(RO_INIT_MULTITHREADED);
		if (FAILED(init))
		{
			EXC("Could not initialize WRL system.");
		}
	#else
		EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
	#endif

	//SandFox::Mesh m;
	//m.Load()

	return 0;
}

int WINAPI WinMain(
    _In_		HINSTANCE	hInstance, 
    _In_opt_	HINSTANCE	hPrevInstance,
    _In_		char*		lpCmdLine, 
    _In_		int			nCmdShow)
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif



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