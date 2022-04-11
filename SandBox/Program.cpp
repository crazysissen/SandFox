
#include <SandFox.h>
#include <SandFox\Window.h>
#include <SandFox\Graphics.h>
#include <SandFox\Input.h>
#include <SandFox\CubeInterpolated.h>
#include <SandFox\MeshDrawable.h>
#include <SandFox\Mesh.h>
#include <SandFox\ParticleStream.h>
#include <SandFox\ImGuiHandler.h>

#include <SandFox\imgui.h>



// Input system

sx::Input input;

constexpr float c_moveSpeed = 0.3f;
constexpr float c_lookSpeed = 0.008f;
constexpr byte c_mouseKey = sx::KeyE;

Mat3 HandleInput()
{
	bool locked = sx::Input::GetMouseLocked();

	std::shared_ptr<sx::Camera> c = sx::Graphics::Get().GetCamera();

	cs::Vec3 move;
	cs::Vec2 look;

	if (sx::Input::KeyPressed(sx::KeyW))		move += cs::Vec3(0, 0, 1);
	if (sx::Input::KeyPressed(sx::KeyA))		move += cs::Vec3(-1, 0, 0);
	if (sx::Input::KeyPressed(sx::KeyS))		move += cs::Vec3(0, 0, -1);
	if (sx::Input::KeyPressed(sx::KeyD))		move += cs::Vec3(1, 0, 0);
	if (sx::Input::KeyPressed(sx::KeySpace))	move += cs::Vec3(0, 1, 0);
	if (sx::Input::KeyPressed(sx::KeyShift))	move += cs::Vec3(0, -1, 0);

	if (sx::Input::KeyDown(c_mouseKey))
	{
		sx::Input::MouseLocked(!locked);
		sx::Input::MouseVisible(locked);
	}

	if (locked)
	{
		look = (cs::Vec2)sx::Input::MousePositionDiff() * c_lookSpeed;
		c->rotation = cs::Vec3(
			cs::fclamp(c->rotation.x - look.y, -cs::c_pi * 0.5f, cs::c_pi * 0.5f),
			cs::fwrap(c->rotation.y - look.x, -cs::c_pi, cs::c_pi),
			0
		);
	}

	Mat3 turn = Mat::rotation3Y(-c->rotation.y);
	move = turn * (move * c_moveSpeed);

	c->position += move; 

	return turn * Mat::rotation3X(-c->rotation.x);
}



int SafeWinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	char*		lpCmdLine,
	int			nCmdShow)
{
	// DXGI debugger

	cs::dxgiInfo::init();



	// Initial setup of base resources

	sx::Window window;
	sx::Graphics graphics;
	cs::Random r;

	window.InitClass(hInstance);
	window.InitWindow(1664, 936, "SandBox"); 

	sx::GraphicsTechnique technique = sx::GraphicsTechniqueImmediate;
	graphics.Init(&window, L"Assets\\Shaders", technique);
	graphics.InitCamera({ 0, 0, 0 }, { 0, 0, 0 }, cs::c_pi * 0.5f);

	input.LoadWindow(&window);

	sx::ImGuiHandler imgui(&graphics);

	sx::Input::MouseLocked(false);

	window.Show();


	
	// Lights

	const int lightCount = 2;

	sx::Graphics::Light lights[lightCount] =
	{
		sx::Graphics::Light::Directional({ -0.1f, -1.0f, 0.1f }, 0.9f, cs::Color(0xFFFFF0)),
		//sx::Graphics::Light::Point({ 0, 0, 0 }, 50.0f),
		sx::Graphics::Light::Spot({ 0, 0, 0 }, { 0, 0, 1 }, 0.6f, 20.0f)
	};
	
	graphics.SetLights(lights, lightCount);
	graphics.SetLightAmbient(cs::Color(0xFFFFFF), 0.2f);



	// Models

 	sx::Mesh m;
	m.Load(L"Assets/Models/MonkeyTexture.obj");

	sx::Prim::MeshDrawable suzannes[50];
	
	for (int i = 0; i < 50; i++)
	{
		suzannes[i].Load(m);
		suzannes[i].SetTransform(
			sx::Transform(
				{ r.Getf(-10, 10), r.Getf(-10, 10), r.Getf(10, 30) },
				{ 0, r.Getf(0, 2 * cs::c_pi), 0 },
				{ 1, 1, 1 }
			)
		);
	}

	sx::Mesh cityMesh;
	cityMesh.Load(L"Assets/Models/Hut.obj");
	sx::Prim::MeshDrawable city(sx::Transform({ 0, -20, 0 }), cityMesh);

	sx::Prim::TexturePlane ground(sx::Transform({ 0, -20, 0 }, { cs::c_pi * 0.5f, 0, 0 }, { 100, 100, 100 }), L"Assets/Textures/Stone.jpg", { 30, 30 });



	// Particles

	struct PData
	{
		float noiseScalar[4]	= { 0.0f };
		cs::Vec3 velocity		= { 0, 0, 0 };
		float dampening			= 0.0f;
		cs::Vec3 acceleration	= { 0, 0, 0 };
		float size				= 1.0f;				/*PAD(12, 0);*/
	};

	struct UpdateInfo
	{
		cs::Vec4 noise[4];
		cs::Vec3 cameraPos;		PAD(4, 0);
	}
	updateInfo;

	cs::NoiseSimplex noise[4 * 3] =
	{ 
		cs::NoiseSimplex(r.GetUnsigned(100000)), cs::NoiseSimplex(r.GetUnsigned(100000)), cs::NoiseSimplex(r.GetUnsigned(100000)),
		cs::NoiseSimplex(r.GetUnsigned(100000)), cs::NoiseSimplex(r.GetUnsigned(100000)), cs::NoiseSimplex(r.GetUnsigned(100000)),
		cs::NoiseSimplex(r.GetUnsigned(100000)), cs::NoiseSimplex(r.GetUnsigned(100000)), cs::NoiseSimplex(r.GetUnsigned(100000)),
		cs::NoiseSimplex(r.GetUnsigned(100000)), cs::NoiseSimplex(r.GetUnsigned(100000)), cs::NoiseSimplex(r.GetUnsigned(100000))
	};

	sx::ParticleStream particles(
		sx::Transform(), 
		L"Assets/Textures/ParticleSofter.png", 
		L"P_CSParticleBasic", 
		sizeof(PData), 
		4, 
		10.0f,
		2.0f,
		1.0f
	);
	sx::Bind::ConstBufferC<UpdateInfo> noiseInfoBuffer(updateInfo, 3, false);

	float noiseTimer = 0.0f;
	float particleTimer = 0.0f;
	float particleTargetTime = 0.0001f;



	// Main message pump and game loop

	cs::Timer timer;
	float dTime = 0.0f;
	float dTimeAverage1 = 0.0f;
	float dTimeAverage5 = 0.0f;
	float fpsAverage1 = 0.0f;
	float fpsAverage5 = 0.0f;

	int exitCode = 0;
	
	for (int frame = 0;; frame++)
	{
#pragma region Performance timing

		static float dTimeAccumulator1 = 0.0f;
		static float dTimeAccumulator5 = 0.0f;
		static int frameCounter1 = 0;
		static int frameCounter5 = 0;

		dTime = timer.Lap();

		dTimeAccumulator1 += dTime;
		dTimeAccumulator5 += dTime;
		frameCounter1++;
		frameCounter5++;

		if (dTimeAccumulator1 > 1.0f)
		{
			fpsAverage1 = frameCounter1;
			dTimeAverage1 = dTimeAccumulator1 / frameCounter1;
			dTimeAccumulator1 = 0;
			frameCounter1 = 0;
		}

		if (dTimeAccumulator5 > 5.0f)
		{
			fpsAverage5 = frameCounter5 / 5.0f;
			dTimeAverage5 = dTimeAccumulator5 / frameCounter5;
			dTimeAccumulator5 = 0;
			frameCounter5 = 0;
		}

#pragma endregion

#pragma region Core stuff
		// Exit code optional evaluates to true if it contains a value
		if (const std::optional<int> optExitCode = window.ProcessMessages())
		{
			//DeInit();
			exitCode = *optExitCode;
			break;
		}

		// Refresh input
		sx::Input::Get().CoreUpdateState();
#pragma endregion

#pragma region Input and camera
		// Update camera and spotlight
		Mat3 orientation = HandleInput();
		Vec3 direction = orientation * Vec3(0, 0, 1);

		lights[1].position = graphics.GetCamera()->position;
		lights[1].direction = direction;
		graphics.SetLights(lights, lightCount);
#pragma endregion

#pragma region Particle updating

		// Update particles

		noiseTimer += dTime;
		for (int i = 0; i < 4; i++)
		{
			updateInfo.noise[i] = Vec4(
				noise[i * 3 + 0].Gen1D(noiseTimer),
				noise[i * 3 + 1].Gen1D(noiseTimer),
				noise[i * 3 + 2].Gen1D(noiseTimer),
				0.0f
			);
		}

		updateInfo.cameraPos = sx::Graphics::Get().GetCamera()->position;

		noiseInfoBuffer.Write(updateInfo);
		noiseInfoBuffer.Bind();

		particleTimer += dTime;
		while (particleTimer > particleTargetTime)
		{
			PData pd =
			{
				{ r.Getf(-1.0f, 1.0f), r.Getf(-1.0f, 1.0f), r.Getf(-1.0f, 1.0f), r.Getf(-1.0f, 1.0f)},
				{ r.Getf(-5, 5), r.Getf(2, 4), r.Getf(-5, 5)},
				0.3f,
				{ 0, -6, 0 }, 
				r.Getf(0.1f, 0.2f)
			};

			particles.CreateParticle({ r.Getf(-60.0f, 60.0f), 10, r.Getf(-60.0f, 60.0f) }, pd.size, & pd);
			particleTimer -= particleTargetTime;
		}

		particles.Update(dTime);

#pragma endregion



		// Draw the frame 

		graphics.FrameBegin(cs::Color(0x301090));
		graphics.ChangeDepthStencil(true, true); 

		//for (int i = 0; i < 50; i++)
		//	suzannes[i].Draw();
		
		city.Draw();
		ground.Draw();

		graphics.ChangeDepthStencil(true, false);
		particles.Draw();

#pragma region Imgui

#define SPACE3 ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		imgui.BeginDraw();

		{
			ImGui::Begin("Info");


			// Main information 
			ImGui::Text("Client");
			ImGui::Spacing();
			int res[] = { window.GetW(), window.GetH() };
			ImGui::Text(technique == sx::GraphicsTechniqueImmediate ? "Technique: Immediate Rendering" : "Technique: Deferred Rendering");
			ImGui::InputInt2("Resolution", res, ImGuiInputTextFlags_ReadOnly);
			SPACE3;

			// Basic performance monitoring
			ImGui::Text("Performance");
			ImGui::Spacing();
			ImGui::InputFloat("Frame time", &dTime, 0, 0, "%.8f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Frame time (1s)", &dTimeAverage1, 0, 0, "%.8f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Frame time (5s)", &dTimeAverage5, 0, 0, "%.8f", ImGuiInputTextFlags_ReadOnly);
			ImGui::Spacing();
			ImGui::InputFloat("FPS (1s)", &fpsAverage1, 0, 0, "%.4f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("FPS (5s)", &fpsAverage5, 0, 0, "%.4f", ImGuiInputTextFlags_ReadOnly);
			ImGui::Spacing();
			ImGui::InputInt("Frame", &frame, 0, 0, ImGuiInputTextFlags_ReadOnly);

			ImGui::End();
		}

		imgui.EndDraw();

#pragma endregion

		graphics.FrameFinalize();
	}

	graphics.DeInit();

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





//#ifndef _RELEASE

	try
	{
		SafeWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
	catch (const cs::Exception& e)
	{
		input.MouseVisible(true);
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& e)
	{
		input.MouseVisible(true);
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		input.MouseVisible(true);
		MessageBoxA(nullptr, "No details", "Unknown Exception", MB_OK | MB_ICONERROR);
	}

//#else
//
//	SafeWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
//
//#endif

	return 0;
}