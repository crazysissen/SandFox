
#include <optional>

#include <SandFox.h>
#include <SandFox\Window.h>
#include <SandFox\Graphics.h>
#include <SandFox\Input.h>

// Loads D3D11 Library for the whole project
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")


int WINAPI WinMain(
	_In_		HINSTANCE	hInstance,
	_In_opt_	HINSTANCE	hPrevInstance,
	_In_		char* lpCmdLine,
	_In_		int			nCmdShow)
{
	sx::Window window;
	
	window.InitClass(hInstance);
	window.InitWindow(1280, 720, "Hello World");

//#if (_WIN32_WINNT >= 0x0A00)
//	wrl::Wrappers::RoInitializeWrapper init(RO_INIT_MULTITHREADED);
//	if (FAILED(init))
//	{
//		EXC("Could not initialize WRL system.");
//	}
//#else
//	EXC_COMCHECK(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
//#endif

	sx::Graphics graphics;
	sx::Input input;

	graphics.Init();
	graphics.InitCamera({ 0, 0, -3 }, { 0, 0, 0 }, cs::c_pi * 0.5f);

	window.Show();



	// Main message pump and game loop

	cs::Timer timer;

	while (true)
	{
		// Exit code optional evaluates to true if it contains a value
		if (const std::optional<int> exitCode = window.ProcessMessages())
		{
			//DeInit();
			return *exitCode;
		}

		float dTime = timer.Lap();

		graphics.FrameBegin(cs::Color(0x7000E0));
		graphics.UpdateCameraMatrix();
		graphics.FrameFinalize();
	}

	return 0;
}