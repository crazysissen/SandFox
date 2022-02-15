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



struct Light
{
	Vec3 position;
	float intensity;

	Vec3 diffuse;
	PAD1(1);

	Vec3 specular;
	PAD2(1);
};

struct SceneInfo
{
	Vec3 cameraPos;
	PAD(1);

	Vec3 ambient;
	int lightCount;

	Light lights[6];
};



Graphics* g_graphics;

prim::TexturePlane* g_plane;

int g_cubeCount;
std::vector<prim::TestCube*>* g_cubes;
std::vector<Vec3> g_cubeRotations;

Vec3 planePos = { 0, 0, -0.5f };
Vec3 turntablePos = { 0, 0, 1.0f };

ibind::ConstBufferP<SceneInfo>* g_sceneInfo;



void Start()
{
	// Cubes

	constexpr int count = 500;
	constexpr float range = 50;

	g_cubeCount = count;

	cs::Random r;

	g_cubes = new std::vector<prim::TestCube*>();

	for (int i = 0; i < count; i++)
	{
		g_cubeRotations.push_back({ r.Getf(-1, 1)* cs::c_pi, r.Getf(-1, 1)* cs::c_pi, r.Getf(-1, 1)* cs::c_pi });

		g_cubes->push_back(new prim::TestCube(
			{ r.Getf(-1, 1) * range, r.Getf(-1, 1) * range, r.Getf(0.1f, 1) * range },
			g_cubeRotations[i],
			{ 1, 1, 1 }
		));
	}



	// Square

	SceneInfo si =
	{
		{ 0, 0, -3 }, 0,
		{ 0.07f, 0.07f, 0.12f },
		1,
		{}
	};

	si.lights[0] =
	{
		{ 0, 0, -2.5f },
		2.5f,
		{ 1, 1, 0.8f }, 0,
		{ 1, 1, 0.8f }, 0
	};

	g_sceneInfo = new ibind::ConstBufferP<SceneInfo>(si, 1, false);
	g_plane = new prim::TexturePlane(Transform(), L"Square3.png");

	g_sceneInfo->Bind();
}

void Update(float dTime)
{
	float xDiff = (float)input::mousePosition().x / window::c_width - 0.5f;

	for (int i = 0; i < g_cubeCount; i++)
	{
		(*g_cubes)[i]->m_transform.ChangeRotation(g_cubeRotations[i] * dTime);
	}

	g_plane->m_transform.ChangeRotation(Vec3(0, -2.0f, 0) * xDiff * dTime);
	g_plane->m_transform.SetPosition(Mat::rotation3Y(g_plane->m_transform.GetRotation().y) * (planePos - turntablePos) + turntablePos);
}

void Draw(float dTime)
{
	g_graphics->FrameBegin(cs::Color(0x000020));
	g_graphics->UpdateCameraMatrix();

	for (prim::TestCube* c : *g_cubes)
	{
		c->Draw();
	}

	g_plane->Draw();

	g_graphics->FrameFinalize();
}

void DeInit()
{
	// Drawable instances
	for (prim::TestCube*& c : *g_cubes)
	{
		delete c;
	}
	delete g_cubes;
	delete g_sceneInfo;
	delete g_plane;

	// Drawables
	_Drawable::ReleaseStatics();

	// DXGI debugger
	std::vector<std::string> messages = cs::dxgiInfo::getMessages();
	for (std::string& m : messages)
	{
		std::cout << m << '\n';
	}
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