
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

constexpr float c_moveSpeed = 20.0f;
constexpr float c_lookSpeed = 0.008f;
constexpr byte c_mouseKey = sx::KeyE;

Mat3 HandleInput(float dTime)
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
		//sx::Input::MouseVisible(locked);
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
	move = turn * (move * c_moveSpeed) * dTime;

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
	window.InitWindow(1920, 1080, "SandBox");

	sx::GraphicsTechnique technique = sx::GraphicsTechniqueDeferred;
	graphics.Init(&window, L"Assets\\Shaders", technique);
	graphics.InitCamera({ 0, 0, 0 }, { 0, 0, 0 }, cs::c_pi * 0.5f);

	input.LoadWindow(&window);

	sx::ImGuiHandler imgui(&graphics);

	sx::Input::MouseLocked(false);

	window.Show();



	// Creating drawables

#pragma region Lights
	
	// Lights

	cs::List<sx::Graphics::Light> lights;
	float ambientLight = 0.2f;
	cs::Color ambientColor(0xFFFFFF);

	lights.Add(sx::Graphics::Light::Directional({ -0.1f, -1.0f, 0.1f }, 0.9f, cs::Color(0xFFFFF0)));
	lights.Add(sx::Graphics::Light::Spot({ 0, 0, 0 }, { 0, 0, 1 }, 0.6f, 20.0f));

	int lightLockIndex = -1;
	Vec3 lightLockOffset = { 0, 0, 0 };
	
	graphics.SetLights(lights.Data(), lights.Size());
	graphics.SetLightAmbient(ambientColor, ambientLight);

#pragma endregion

#pragma region Models

	// Models

	sx::Mesh mSphere1;
	mSphere1.Load(L"Assets/Models/Sphere1.obj");
	sx::Prim::MeshDrawable sphere1(sx::Transform({ 0, 0, 10 }), mSphere1);

	sx::Mesh mTerrain1;
	mTerrain1.Load(L"Assets/Models/Terrain1.obj");
	sx::Prim::MeshDrawable terrain1(sx::Transform({ 0, -20, 0 }, { 0, 0, 0 }, { 10, 5, 10 }), mTerrain1);

	sx::Mesh mWatchtower;
	mWatchtower.Load(L"Assets/Models/Watchtower.obj");
	sx::Prim::MeshDrawable watchtower(sx::Transform({ 0, -20, 0 }, { 0, 0, 0 }, { 2, 2, 2 }), mWatchtower);

 	sx::Mesh mMonkey; 
	mMonkey.Load(L"Assets/Models/MonkeyTexture.obj");
	sx::Prim::MeshDrawable suzannes[50];
	for (int i = 0; i < 50; i++)
	{
		suzannes[i].Load(mMonkey);
		suzannes[i].SetTransform(
			sx::Transform(
				{ r.Getf(-10, 10), r.Getf(-10, 10), r.Getf(10, 30) },
				{ 0, r.Getf(0, 2 * cs::c_pi), 0 },
				{ 1, 1, 1 }
			)
		);
	}

	//sx::Prim::TexturePlane ground(sx::Transform({ 0, -20, 0 }, { cs::c_pi * 0.5f, 0, 0 }, { 100, 100, 100 }), L"Assets/Textures/Stone.jpg", { 30, 30 });

#pragma endregion

#pragma region Particles

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
	float particleTargetTime = 0.001f;
	
	Vec3 particleSpawn = { 15, 0, 0 };
	Vec3 particleSpawnArea = { 3, 0, 3 }; // Maximum distance in each direction for particle spawn
	Vec3 particleVelocity = { 0, 5, 0 };
	Vec3 particleVelocityVariability = { 2, 2, 2 };
	Vec3 particleAcceleration = { 0, -5, 0 };
	float particleSizeMin = 0.2f;
	float particleSizeMax = 0.3f;
	float particleDampening = 0.2f;

#pragma endregion



	// Main message pump and game loop

	cs::Timer timer;
	float dTime = 0.0f;
	float dTimeAverage16 = 0.0f;
	float dTimeAverage256 = 0.0f;
	float fpsAverage16 = 0.0f; 
	float fpsAverage256 = 0.0f;

	int exitCode = 0;
	
	for (int frame = 0;; frame++)
	{
#pragma region Performance timing

		static float dTimeAccumulator16 = 0.0f;
		static float dTimeAccumulator256 = 0.0f;
		static cs::Queue<float> dTimeQueue16;
		static cs::Queue<float> dTimeQueue256;

		dTime = timer.Lap();

		dTimeAccumulator16 += dTime;
		dTimeAccumulator256 += dTime;
		dTimeQueue16.Push(dTime);
		dTimeQueue256.Push(dTime);

		if (dTimeQueue16.Size() == 16)
		{
			dTimeAccumulator16 -= dTimeQueue16.Pop();
		}

		if (dTimeQueue256.Size() == 256)
		{
			dTimeAccumulator256 -= dTimeQueue256.Pop();
		}

		dTimeAverage16 = dTimeAccumulator16 / dTimeQueue16.Size();
		dTimeAverage256 = dTimeAccumulator256 / dTimeQueue256.Size();
		fpsAverage16 = 1.0f / dTimeAverage16;
		fpsAverage256 = 1.0f / dTimeAverage256;

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

#pragma region Input, light, and camera
		// Update camera and spotlight
		Mat3 orientation = HandleInput(dTime);
		Vec3 direction = orientation * Vec3(0, 0, 1);

		if (lightLockIndex >= 0 && lightLockIndex < lights.Size())
		{
			lights[lightLockIndex].position = graphics.GetCamera()->position + orientation * lightLockOffset;
			lights[lightLockIndex].direction = direction;
		}
		graphics.SetLights(lights.Data(), lights.Size());
		graphics.SetLightAmbient(ambientColor, ambientLight);
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
				particleVelocity + Vec3(r.Getf(-1, 1), r.Getf(-1, 1), r.Getf(-1, 1)) % particleVelocityVariability,
				particleDampening,
				particleAcceleration,
				r.Getf(particleSizeMin, particleSizeMax)
			};

			particles.CreateParticle(particleSpawn + particleSpawnArea % Vec3(r.Getf(-1.0f, 1.0f), r.Getf(-1.0f, 1.0f), r.Getf(-1.0f, 1.0f)), pd.size, &pd);
			particleTimer -= particleTargetTime;
		}

		particles.Update(dTime);

#pragma endregion

		// Draw the frame 

		graphics.FrameBegin(cs::Color(0x301090));
		graphics.ChangeDepthStencil(true, true); 

		//for (int i = 0; i < 50; i++)
		//	suzannes[i].Draw();
		
		//ground.Draw();
		watchtower.Draw();
		sphere1.Draw();
		terrain1.Draw();

		graphics.FrameComposite();

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
			ImGui::InputFloat("Delta time", &dTime, 0, 0, "%.8f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Delta time (16)", &dTimeAverage16, 0, 0, "%.8f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Delta time (256)", &dTimeAverage256, 0, 0, "%.8f", ImGuiInputTextFlags_ReadOnly);
			ImGui::Spacing();
			ImGui::InputFloat("FPS (16)", &fpsAverage16, 0, 0, "%.4f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("FPS (256)", &fpsAverage256, 0, 0, "%.4f", ImGuiInputTextFlags_ReadOnly);
			ImGui::Spacing();
			ImGui::InputInt("Frame", &frame, 0, 0, ImGuiInputTextFlags_ReadOnly);
			SPACE3;

			// Input
			ImGui::Text("Input");
			ImGui::Spacing();
			Point mp = sx::Input::MousePosition();
			ImGui::InputInt2("Mouse pos", (int*)&mp, ImGuiInputTextFlags_ReadOnly);

			ImGui::End();
		}

		{
			ImGui::Begin("Controls");

			if (ImGui::BeginTabBar("What"))
			{
				if (ImGui::BeginTabItem("Graphics"))
				{
					graphics.DrawGraphicsImgui();
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Lights"))
				{
					if (ImGui::BeginListBox("Lights"))
					{
						if (ImGui::Button("Ambient Light"))
						{
							ImGui::OpenPopup("AmbientLight");
						}

						if (ImGui::BeginPopupContextWindow("AmbientLight"))
						{
							ImGui::ColorEdit3("Color", (float*)&ambientColor);
							ImGui::DragFloat("Intensity", &ambientLight, 0.01f, 0.0f, 5.0f);

							ImGui::EndPopup();
						}

						for (int i = 0; i < lights.Size(); i++)
						{
							string title = "";
							if (lightLockIndex == i) title += "(A) ";
							title += "Light ";
							title += std::to_string(i);
							if (lights[i].type == sx::LightTypeDirectional) title += " (Directional)";
							if (lights[i].type == sx::LightTypePoint) title += " (Point)";
							if (lights[i].type == sx::LightTypeSpot) title += " (Spot)";

							if (ImGui::Button(title.c_str()))
							{
								ImGui::OpenPopup(title.c_str());
							}

							if (ImGui::BeginPopupContextWindow(title.c_str()))
							{
								if (lights[i].type != sx::LightTypeDirectional)
								{
									ImGui::DragFloat3("Position", (float*)&lights[i].position, 0.05f);
								}
								if (lights[i].type != sx::LightTypePoint)
								{
									ImGui::DragFloat3("Direction", (float*)&lights[i].direction, 0.05f);
								}
								if (lights[i].type == sx::LightTypeSpot)
								{
									ImGui::SliderFloat("Spread", (float*)&lights[i].spreadDotLimit, 0.0f, 1.0f);
								}

								ImGui::ColorEdit3("Color", (float*)&lights[i].color);
								ImGui::DragFloat("Intensity", (float*)&lights[i].intensity, 0.01f, 0.0f, 100.0f);

								if (ImGui::Button("Anchor"))
								{
									lightLockIndex = i;
								}

								if (ImGui::Button("Remove Light"))
								{
									lights.Remove(i);
									i--;
								}

								ImGui::EndPopup();
							}

						}

						ImGui::EndListBox();
					}

					if (ImGui::Button("Add Light"))
					{
						ImGui::OpenPopup("AddLight"); 
					}

					if (ImGui::BeginPopupContextWindow("AddLight"))
					{
						Vec3 pos = sx::Graphics::Get().GetCamera()->position;

						if (ImGui::Button("Directional Light"))
						{
							lights.Add(sx::Graphics::Light::Directional(direction));
						}

						if (ImGui::Button("Point Light"))
						{
							lights.Add(sx::Graphics::Light::Point(pos));
						}

						if (ImGui::Button("Spot Light"))
						{
							lights.Add(sx::Graphics::Light::Spot(pos, direction));
						}

						ImGui::Text("Light will, as applicable,");
						ImGui::Text("appear in the camera's position,");
						ImGui::Text("facing the same way.");

						ImGui::EndPopup();
					}

					SPACE3;
					if (ImGui::Button("Release Anchor")) lightLockIndex = -1;
					ImGui::InputFloat3("Anchor Offset", (float*)&lightLockOffset);

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

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