
#include <SandFox.h>
#include <SandFox\Graphics.h>
#include <SandFox\Window.h>
#include <SandFox\Input.h>
#include <SandFox\CubeInterpolated.h>
#include <SandFox\MeshDrawable.h>
#include <SandFox\Mesh.h>
#include <SandFox\ParticleStream.h>
#include <SandFox\ImGuiHandler.h>
#include <SandFox\Debug.h>
#include <SandFox\Cubemap.h>
#include <SandFox\MirrorDrawable.h>

#include <SandFox\ImGui\imgui.h>



// ---------------------- SandBox
// 
// Welcome!
// 
// This is merely a sandbox project containing a lot of
// demonstration and testing code, as well as UI. All actual
// DX11 and other core functionality happens in the SandFox
// project, which compiles into a dynamic library automatically
// used by this project.
// 
// MAKE SURE THAT PROJECT IS BUILT BEFORE BUILDING THIS!
// Should be done automatically by reference, but just in case.
// 
// The other library used, CHSL, is my own library, and 
// contains mainly helper functionality and general-purpose
// classes that I use across many projects.
// 
// To switch rendering technique, modify the define below.
// Either	sx::GraphicsTechniqueImmediate
// or..		sx::GraphicsTechniqueDeferred
//

#define TECHNIQUE sx::GraphicsTechniqueImmediate



// Input system

#pragma region Input

sx::Input input;

constexpr float c_moveSpeed = 20.0f;
constexpr float c_slowModifier = 0.1f;
constexpr float c_lookSpeed = 0.008f;
constexpr byte c_mouseKey = sx::KeyE;

Mat3 HandleInput(float dTime)
{
	bool locked = sx::Input::GetMouseLocked(); 

	std::shared_ptr<sx::Camera> c = sx::Graphics::Get().GetCamera();  

	cs::Vec3 move;
	cs::Vec2 look;

	if (locked) 
	{
		if (sx::Input::KeyPressed(sx::KeyW))		move += cs::Vec3(0, 0, 1);
		if (sx::Input::KeyPressed(sx::KeyA))		move += cs::Vec3(-1, 0, 0);
		if (sx::Input::KeyPressed(sx::KeyS))		move += cs::Vec3(0, 0, -1);
		if (sx::Input::KeyPressed(sx::KeyD))		move += cs::Vec3(1, 0, 0);
		if (sx::Input::KeyPressed(sx::KeySpace))	move += cs::Vec3(0, 1, 0);
		if (sx::Input::KeyPressed(sx::KeyShift))	move += cs::Vec3(0, -1, 0);

		if (sx::Input::KeyPressed(sx::KeyControl)) move *= c_slowModifier;
	}

	if (sx::Input::KeyDown(c_mouseKey))
	{
		sx::Input::MouseLocked(!locked);
		//sx::Input::MouseVisible(locked);
	}

	if (locked)
	{
		look = (cs::Vec2)sx::Input::MousePositionDiff() * c_lookSpeed;
		c->rotation = cs::Vec3(
			cs::fclamp(c->rotation.x + look.y, -cs::c_pi * 0.5f, cs::c_pi * 0.5f), 
			cs::fwrap(c->rotation.y + look.x, -cs::c_pi, cs::c_pi),
			0
		);
	}

	Mat3 turn = Mat::rotation3Y(c->rotation.y);
	move = turn * (move * c_moveSpeed) * dTime; 

	c->position += move; 

	return turn * Mat::rotation3X(c->rotation.x);
}

inline bool CullFrustum(const cs::Box& box, void* data) 
{
	return ((cs::Frustum*)data)->IntersectsFuzzy(box); 
}

#pragma endregion



sx::Debug debug;

int SafeWinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	char*		lpCmdLine,
	int			nCmdShow) 
{
	// Variables

	float fov = cs::c_pi * 0.5f; 
	float nearClip = 0.01f;
	float farClip = 1000.0f;
	cs::Point aspectRatio = { 16, 9 }; 

	bool frustumEnable = true;
	float frustumFov = cs::c_pi * 0.4f;
	float frustumNearClip = 0.01f;
	float frustumFarClip = 1000.0f;
	cs::Point frustumAspectRatio = { 16, 9 }; 

	int monkeyDisplayCount = 0; // Set later to max 



	// Initial setup of base resources 

	 
	sx::Window window;
	window.InitClass(hInstance); 
 	window.InitWindow(hInstance, 1920, 1080, "SandBox", true);

	sx::Graphics graphics;
	sx::GraphicsTechnique technique = TECHNIQUE;
	graphics.Init(&window, L"Assets\\Shaders", technique, &debug);
	graphics.InitCamera({ 0, 0, 0 }, { 0, 0, 0 }, fov, nearClip, farClip); 

	// Input created statically
	input.LoadWindow(&window);
	input.MouseLocked(false); 

	sx::ImGuiHandler *imgui = new sx::ImGuiHandler(&graphics);

	window.Show(); 
	debug.CreateConsole(); 
	
	cs::Random r;
	cs::ViewFrustum frustum(Vec3(0, 0, 0), Vec3(0, 0, 0), frustumNearClip, frustumFarClip, frustumFov); 



	// Creating drawables

#pragma region Lights
	
	// Lights

	float ambientLight = 0.2f;
	cs::Color ambientColor(0xFFFFFF);

	cs::List<sx::LightID> lights;

	sx::LightHandler lightHandler; 
	lightHandler.Init(sx::TextureQualityHighest, technique, ambientColor, ambientLight);

	lights.Add(lightHandler.AddDirectionalShadowed({ 0.75f, 1.95f, 0.0f }, 50.0f, 1.8f, cs::Color(0xFFFFF0), 0.05f, 500.0f));
	lights.Add(lightHandler.AddSpotShadowed({ 0, 0, 0 }, { 0, 0, 0 }, 1.0f, 20.0f, cs::Color(0xFFFFFF), 0.01f, 100.0f));
	lightHandler.SetAmbient(ambientColor, ambientLight); 

	int lightLockIndex = -1;
	Vec3 lightLockOffset = { 0, 0, 0 };

	Vec3 samplePoints[] =
	{
		{ 100, 0, 100 },
		{ -100, 0, 100 },
		{ -100, 0, -100 },
		{ 100, 0, -100 }
	};

	lightHandler.SetFocalPoint(Vec3(0, 0, 0));

#pragma endregion

#pragma region Dynamic area map

	sx::TextureQuality areaMapQuality = sx::TextureQualityLower;
	unsigned int areaMapWidth = sx::GetTextureQuality(areaMapQuality);
	int areaMapMode = 1;
	float areaMapFrequency = 60.0f;
	float areaMapTarget = 1.0f / areaMapFrequency;
	float areaMapTime = 0.0f;
	bool areaMapOnce = false;

	sx::CubemapDynamic areaMap;
	areaMap.Load({ -10, 0, 0 }, areaMapQuality, sx::RegSRVCubemap);

#pragma endregion

#pragma region Models

	// Models

	sx::Cubemap skyboxTexture;
	skyboxTexture.Load(sx::RegSRVCubemap, L"Assets/Textures/Cubemaps/Clouds2.png"); 
	sx::CubemapDrawable skybox(sx::Transform({}, {}, { 10, 10, 10 }), &skyboxTexture, false); 

	sx::Mesh sphereMesh;
	sphereMesh.Load(L"Assets/Models/Sphere1.obj");
	sx::Prim::MeshDrawable sphere(sx::Transform({ 0, 0, 10 }), sphereMesh, true); 
	sphere.SetUVScaleAll(Vec2(2, 2)); 

	sx::Mesh sphereMesh2;
	sphereMesh2.Load(L"Assets/Models/Sphere2.obj");
	sx::Prim::MeshDrawable sphere2(sx::Transform({ 0, 0, 20 }), sphereMesh2, true); 
	sphere2.SetUVScaleAll(Vec2(2, 2));

	sx::Mesh icosahedronMesh;
	icosahedronMesh.Load(L"Assets/Models/Icosahedron.obj");
	sx::Prim::MeshDrawable icosahedron(sx::Transform({ 10, 0, 0 }), icosahedronMesh, true); 
	icosahedron.SetUVScaleAll(Vec2(8, 8));

	sx::Mesh terrainMesh;
	terrainMesh.Load(L"Assets/Models/Terrain2.obj");
	sx::Prim::MeshDrawable terrain(sx::Transform({ 0, -20, 0 }, { 0, 0, 0 }, { 2, 2, 2 }), terrainMesh, false);
	terrain.SetUVScaleAll(Vec2(40, 40)); 

	sx::Mesh watchtowerMesh;
	watchtowerMesh.Load(L"Assets/Models/Watchtower.obj");
	sx::Prim::MeshDrawable watchtower(sx::Transform({ 0, -21.5f, 0 }, { 0, 0, 0 }, { 2, 2, 2 }), watchtowerMesh, false); 

	sx::Mesh monkeyMesh;
	monkeyMesh.Load(L"Assets/Models/Monkey1.obj");
	sx::Prim::MeshDrawable monkey1(sx::Transform({ -15, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }), monkeyMesh, true);
	sx::Prim::MeshDrawable monkey2(sx::Transform({ -15, 0, 5 }, { 0, 0, 0 }, { 1, 1, 1 }), monkeyMesh, true);
	sx::Prim::MeshDrawable monkey3(sx::Transform({ -15, 0, -5 }, { 0, 0, 0 }, { 1, 1, 1 }), monkeyMesh, true);
	sx::Prim::MeshDrawable monkey4(sx::Transform({ -15, 5, 0 }, { 0, 0, 0 }, { 1, 1, 1 }), monkeyMesh, true);



	// Mirror

	sx::Prim::MeshDrawable mirrorInternal(sx::Transform({ -10, 0, 0 }), sphereMesh, true); 
	sx::MirrorDrawable mirror;
	mirror.Load(&mirrorInternal, &areaMap, true); 



	// Texture plane

	sx::Prim::TexturePlane plane(sx::Transform({ 10, 10, 0 }, { 0.5f, 0.5f, 0.0f }), L"Assets/Textures/Square.jpg");

	

	// DrawQueue

	sx::DrawQueue dq;
	dq.Init(false);



	// Monkeys

	cs::Octree<sx::Prim::MeshDrawable*>* monkeyTree = new cs::Octree<sx::Prim::MeshDrawable*>(
		cs::Box(-100, -50, 50, 200, 100, 200),
		8
	);

	const int c_monkeyCount = 1000; 
	monkeyDisplayCount = 0;

 	sx::Mesh mMonkey; 
	mMonkey.Load(L"Assets/Models/MonkeyTexture.obj");
	sx::Prim::MeshDrawable* suzannes = new sx::Prim::MeshDrawable[c_monkeyCount];
	cs::Box baseBox(-mMonkey.vertexFurthest, -mMonkey.vertexFurthest, -mMonkey.vertexFurthest, mMonkey.vertexFurthest * 2, mMonkey.vertexFurthest * 2, mMonkey.vertexFurthest * 2);
	for (int i = 0; i < c_monkeyCount; i++)
	{
		suzannes[i].Load(mMonkey); 
		suzannes[i].SetTransform(
			sx::Transform(
				{ r.Getf(-100, 100), r.Getf(-50, 50), r.Getf(50, 250) }, 
				{ 0, r.Getf(0, 2 * cs::c_pi), 0 },
				{ 1, 1, 1 }
			)
		);

		monkeyTree->Add(&suzannes[i], cs::Box(baseBox.position + suzannes[i].GetTransform().GetPosition(), baseBox.size));

		dq.AddMain(&suzannes[i]);
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

	sx::Bind::ConstBuffer noiseInfoBuffer(sx::RegCBVUser0, &updateInfo, sizeof(UpdateInfo));

	int spawnRate = 1000;
	float particleTargetTime = 1.0f / spawnRate;

	float noiseTimer = 0.0f;
	float particleTimer = 0.0f;
	
	Vec3 particleSpawn = { 0, -20, -20 };
	Vec3 particleSpawnArea = { 2, 0, 2 }; // Maximum distance in each direction for particle spawn
	Vec3 particleVelocity = { 0, 30, 0 };
	Vec3 particleVelocityVariability = { 2, 2, 2 };
	Vec3 particleAcceleration = { 0, -9.81f, 0 };
	float particleSizeMin = 0.15f;
	float particleSizeMax = 0.2f;
	float particleDampening = 0.2f;

#pragma endregion

#pragma region Tesselation

	float nearTesselation = 1.0f;
	float nearDistance = 1.0f;

	float overrideTesselation = 0.0f;

#pragma endregion



	// -------------------------------------- Main message pump and game loop

	cs::Timer timer; 
	int exitCode = 0;
	
	for (int frame = 0;; frame++)
	{
#pragma region Performance timing

		static float dTime = 0.0f;
		static float dTimeAverage16 = 0.0f;
		static float dTimeAverage256 = 0.0f;
		static float fpsAverage16 = 0.0f;
		static float fpsAverage256 = 0.0f;

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

		if (frame % 32 == 0)
		{
			std::string s = std::to_string(fpsAverage256) + "\n";
			OutputDebugStringA(s.c_str());
		}

#pragma endregion

#pragma region Core stuff
		debug.ClearFrameTrace(); 

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

#pragma region Input/light/camera
		// Update camera and spotlight
		Mat3 orientation = HandleInput(dTime);
		Vec3 direction = orientation * Vec3(0, 0, 1);
		Vec3 position = graphics.GetCamera()->position;

		if (lightLockIndex >= 0 && lightLockIndex < lights.Size())
		{
			sx::Light& l = lightHandler.GetLight(lights[lightLockIndex]); 

			l.position = position + orientation * lightLockOffset; 
			l.direction = direction;
			l.angles = sx::Graphics::Get().GetCamera()->rotation;

			lightHandler.UpdateMap(lights[lightLockIndex]);
		}

		lightHandler.SetAmbient(ambientColor, ambientLight);

		frustum.SetPosition(position);
		frustum.SetViewDirection(orientation);

		skybox.transform.SetPosition(sx::Graphics::Get().GetCamera()->position);
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

		noiseInfoBuffer.Write(&updateInfo);
		noiseInfoBuffer.Bind(sx::BindStageCS);

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

#pragma region Transformation

		static float monkeyTimer = 0.0f;
		static cs::NoiseSimplex monkeyNoise[3] = { cs::NoiseSimplex(1), cs::NoiseSimplex(2), cs::NoiseSimplex(3) };
		monkeyTimer += dTime;

		sx::Transform t;
		
		t = monkey2.GetTransform();
		t.ChangeRotation(cs::Vec3(monkeyNoise[0].Gen1D(monkeyTimer), monkeyNoise[1].Gen1D(monkeyTimer), monkeyNoise[2].Gen1D(monkeyTimer)) * dTime * 2);
		monkey2.SetTransform(t);

		t = monkey3.GetTransform();
		t.SetScale(cs::Vec3(1.0f + 0.5f * monkeyNoise[0].Gen1D(monkeyTimer), 1.0f + 0.5f * monkeyNoise[1].Gen1D(monkeyTimer), 1.0f + 0.5f * monkeyNoise[2].Gen1D(monkeyTimer)));
		monkey3.SetTransform(t);

		t = monkey4.GetTransform();
		t.SetPosition(cs::Vec3(-10 - std::cos(monkeyTimer) * 5, 5, std::sin(monkeyTimer) * 5));
		monkey4.SetTransform(t);

		t = sphere2.GetTransform();
		t.ChangeRotation(cs::Vec3(0, dTime * 0.4f, 0));
		sphere2.SetTransform(t);

#pragma endregion



		// ---------------------------------- Draw the frame 

		dq.Clear();

		if (frustumEnable) 
		{
			monkeyTree->Search(&frustum, CullFrustum, CullFrustum);
			int i = 0;
			for (sx::Prim::MeshDrawable* m : *monkeyTree)
			{
				if (i++ >= monkeyDisplayCount)
				{
					break;
				}

				dq.AddMain(m);
			}
		}
		else
		{
			for (int i = 0; i < std::min(monkeyDisplayCount, c_monkeyCount); i++)
			{
				dq.AddMain(&suzannes[i]);
			}
		}



		// Add drawables

		dq.AddPre(&skybox);
		dq.AddPre(&mirror);
		dq.AddMain(&sphere);
		dq.AddMain(&terrain);
		dq.AddMain(&watchtower); 
		dq.AddMain(&icosahedron); 
		dq.AddMain(&sphere2);
		dq.AddMain(&monkey1);
		dq.AddMain(&monkey2);
		dq.AddMain(&monkey3);
		dq.AddMain(&monkey4);
		dq.AddPost(&particles);

		dq.AddMain(&plane); 



		// Update shadow and area maps 

		for (int i = 0; i < lights.Size(); i++)
		{
			lightHandler.UpdateMap(lights[i]);
		}

		lightHandler.Update(&dq); 
		lightHandler.UpdateLightInfo();
		lightHandler.BindMaps();

		switch (areaMapMode)
		{
		case 0:
			areaMap.Draw(&dq);
			break;

		case 1:
			areaMapTime += dTime;
			if (areaMapTime > areaMapTarget)
			{
				areaMapTime -= areaMapTarget;
				areaMap.Draw(&dq);
			}
			break;

		case 2:
			if (areaMapOnce)
			{
				areaMap.Draw(&dq);
				areaMapOnce = false;
			}
			break;
		}



		// Drawing of scene and then UI

		graphics.DrawFrame(&dq);

#pragma region Imgui

#define SPACE3 ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); 

		imgui->BeginDraw();

		// Info window
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

		// Controls window
		{
			ImGui::Begin("Controls");

			if (ImGui::BeginTabBar("ControlsTabs"))
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
							sx::Light& l = lightHandler.GetLight(lights[i]);

							string title = "";
							if (lightLockIndex == i) title += "(A) ";
							title += "Light ";
							title += std::to_string(i);
							if (l.type == sx::LightTypeDirectional) title += " (Directional)";
							if (l.type == sx::LightTypePoint) title += " (Point)";
							if (l.type == sx::LightTypeSpot) title += " (Spot)";

							if (ImGui::Button(title.c_str()))
							{
								ImGui::OpenPopup(title.c_str());
							}

							if (ImGui::BeginPopupContextWindow(title.c_str()))
							{
								bool changed = false;

								if (l.type != sx::LightTypeDirectional)
								{
									ImGui::DragFloat3("Position", (float*)&l.position, 0.05f);
									changed = true;
								}
								if (l.type != sx::LightTypePoint)
								{
									if (ImGui::DragFloat3("Rotation", (float*)&l.angles, 0.05f))
									{
										l.direction = (cs::Mat::rotation3Y(l.angles.y) * cs::Mat::rotation3X(l.angles.x)) * Vec3(0, 0, 1);
										changed = true;
									}
								}
								if (l.type == sx::LightTypeSpot)
								{
									if (ImGui::SliderFloat("Spread", (float*)&l.fov, 0.01f, 3.0f)) 
									{
										l.spreadDotLimit = std::cosf(l.fov * 0.5f);
										changed = true;
									}
								}

								ImGui::ColorEdit3("Color", (float*)&l.color);
								ImGui::DragFloat("Intensity", (float*)&l.intensity, 0.01f, 0.0f, 100.0f);

								if (ImGui::Button("Anchor"))
								{
									lightLockIndex = i;
								}

								if (ImGui::Button("Remove Light"))
								{
									changed = false;
									lights.Remove(i);
									i--;
								}

								if (changed)
								{
									lightHandler.UpdateMap(lights[i]);
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
							lights.Add(lightHandler.AddDirectional(direction));
						}

						if (ImGui::Button("Point Light"))
						{
							lights.Add(lightHandler.AddPoint(pos));
						}

						if (ImGui::Button("Spot Light"))
						{
							lights.Add(lightHandler.AddSpotShadowed(pos, direction));
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

				if (ImGui::BeginTabItem("World"))
				{
					ImGui::Text("Suzanne Heads");
					ImGui::TextWrapped("Note: Display Count doesn't change the Octree structure, so Octree culling is unaffected.");
					ImGui::SliderInt("Display Count", &monkeyDisplayCount, 0, c_monkeyCount);
					SPACE3;

					ImGui::Text("Particle System");
					ImGui::DragFloat3("Spawn position", (float*)&particleSpawn, 0.05f);
					ImGui::DragFloat3("Spawn area", (float*)&particleSpawnArea, 0.01f);
					ImGui::DragFloat3("Velocity", (float*)&particleVelocity, 0.05f);
					ImGui::DragFloat3("V Randomness", (float*)&particleVelocityVariability, 0.05f);
					ImGui::DragFloat3("Acceleration", (float*)&particleAcceleration, 0.05f);
					ImGui::DragFloat("Dampening", (float*)&particleDampening, 0.01f);
					ImGui::DragFloat("Min Size", (float*)&particleSizeMin, 0.001f, 0.001f, particleSizeMax - 0.001f);
					ImGui::DragFloat("Max Size", (float*)&particleSizeMax, 0.001f, particleSizeMin + 0.001f);
					ImGui::Spacing();
					if (ImGui::DragInt("Spawn Rate", &spawnRate, 5)) particleTargetTime = 1.0f / spawnRate;

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("View"))
				{
					std::shared_ptr<sx::Camera> c = graphics.GetCamera();

					ImGui::Text("Camera");
					if (ImGui::SliderFloat("FOV", &fov, 0.0f, cs::c_pi))									{ c->SetFOV(fov); }
					if (ImGui::InputInt2("Aspect Ratio", (int*)&aspectRatio))								{ c->SetAspectRatio((float)aspectRatio.x / (float)aspectRatio.y); }
					if (ImGui::DragFloat("Near Clip", &nearClip, 0.01f, 0.0f, farClip) ||
						ImGui::DragFloat("Far Clip", &farClip, 1.0f, std::max(nearClip, 10.0f), 1000.0f))	{ c->SetClip(nearClip, farClip); }

					SPACE3;

					ImGui::Text("Frustum");
					ImGui::Checkbox("Frustum Enable", &frustumEnable);
					if (ImGui::SliderFloat("Frustum FOV", &frustumFov, 0.0f, cs::c_pi) ||							
						ImGui::InputInt2("Frustum Aspect Ratio", (int*)&frustumAspectRatio))					{ frustum.SetViewAngle(frustumFov, (float)aspectRatio.x / (float)aspectRatio.y); }
					if (ImGui::DragFloat("Frustum Near Clip", &frustumNearClip, 0.01f, 0.0f, frustumFarClip))	{ frustum.SetNearPlane(frustumNearClip); }
					if (ImGui::DragFloat("Frustum Far Clip", &frustumFarClip, 1.0f, frustumNearClip, 1000.0f))	{ frustum.SetFarPlane(frustumFarClip); }
				
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Misc"))
				{
					ImGui::Text("Dynamic Area Map");
					ImGui::LabelText("Size", "%i x %i", areaMapWidth, areaMapWidth);

					int mode = 0;
					ImGui::Combo("Mode", &areaMapMode, "Continuous\0Semi-Continuous\0Manual", 3);

					switch (areaMapMode)
					{
					case 1:
						if (ImGui::DragFloat("Frequency", &areaMapFrequency, 0.5f, 1.0f, 100.0f))
						{
							areaMapTarget = 1.0f / areaMapFrequency;
						}
						break;

					case 2:
						if (ImGui::Button("Update Area Map"))
						{
							areaMapOnce = true;
						}
						break;

					default:
						break;
					}

					SPACE3;



					if (ImGui::Button("Generate debug message"))
					{
						sx::Debug::PushMessage(sx::DebugLevelDebug, "Test message!");
					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		// Debug console
		debug.DrawConsole();

		imgui->EndDraw(); 

#pragma endregion

		graphics.Present();
	}

	delete imgui;

	graphics.DeInit(); 
	window.DeInitWindow(); 

	delete monkeyTree; 
	delete[] suzannes;

	return exitCode;
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


	debug.Init(false);

	int exitCode = 0;

#if 1

	try
	{
		exitCode = SafeWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
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

#else

	SafeWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

#endif



	//debug.DumpDevice();
	debug.DeInit();

	return exitCode;
}