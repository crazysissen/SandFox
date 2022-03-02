
#include <SandFox.h>
#include <SandFox\Window.h>
#include <SandFox\Graphics.h>
#include <SandFox\Input.h>
#include <SandFox\CubeInterpolated.h>
#include <SandFox\MeshDrawable.h>
#include <SandFox\Mesh.h>






int WINAPI WinMain(
	_In_		HINSTANCE	hInstance,
	_In_opt_	HINSTANCE	hPrevInstance,
	_In_		char* lpCmdLine,
	_In_		int			nCmdShow)
{
	sx::Window window;
	sx::Graphics graphics;
	sx::Input input;
	
	window.InitClass(hInstance);
	graphics.SetShaderDirectory(L"Assets\\Shaders");
	window.InitWindow(1280, 720, "Hello World");

	graphics.Init();
	graphics.InitCamera({ 0, 0, -3 }, { 0, 0, 0 }, cs::c_pi * 0.5f);

	window.Show();



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

	sx::Bind::ConstBufferP<SceneInfo>* g_sceneInfo;

	SceneInfo si =
	{
		{ 0, 0, -3 }, 0,
		{ 0.15f, 0.15f, 0.2f },
		1,
		{}
	};

	si.lights[0] =
	{
		{ 0, 0, -2.5f },
		4.5f,
		{ 1, 1, 0.8f }, 0,
		{ 1, 1, 0.8f }, 0
	};

	g_sceneInfo = new sx::Bind::ConstBufferP<SceneInfo>(si, 1, false);
	g_sceneInfo->Bind();



	sx::Prim::TestCube cube(
		Vec3(0, 0, 0),
		Vec3(1, 1, 0),
		Vec3(1, 1, 1)
	);


 	sx::Mesh m;
	m.Load(L"Assets/Models/City.obj");

	sx::Prim::MeshDrawable suzanne(
		sx::Transform(
			{ 0, 0, 0 },
			{ 0, 0, 0 },
			{ 1, 1, 1 }
		),
		m
	);


	// Main message pump and game loop

	cs::Timer timer;

	for (uint i = 0;; i++)
	{
		// Exit code optional evaluates to true if it contains a value
		if (const std::optional<int> exitCode = window.ProcessMessages())
		{
			//DeInit();
			return *exitCode;
		}

		float dTime = timer.Lap();

		sx::Transform t = suzanne.GetTransform(); 
		t.ChangeRotation(Vec3(1, 0.5f, 0.8f) * dTime);
		suzanne.SetTransform(t);

		graphics.FrameBegin(cs::Color(0x301090));
		graphics.UpdateCameraMatrix();

		//cube.Draw();
		suzanne.Draw();

		graphics.FrameFinalize();
	}

	return 0;
}