 _____                 _______         
/  ___|               | |  ___|        
\ `--.  __ _ _ __   __| | |_ _____  __ 
 `--. \/ _` | '_ \ / _` |  _/ _ \ \/ / 
/\__/ / (_| | | | | (_| | || (_) >  <  
\____/ \__,_|_| |_|\__,_\_| \___/_/\_\ 

(c) Sixten Schelin 2022
Distributed freely, do whatever with it. By using this framework you acknowlege that the original creator(s) hold no responsibility or control over the usage, or results/effects related to usage usage, of the software included. 
DearImGui is not affiliated with the SandFox framework nor creator(s) of the SandFox framework.

Lightweight 3D graphics framework targeting Windows (x64).
Currently in the early stages of development.


	
	---



Visual Studio copypasta:

If in Solution/Libraries folder:
|	
|	General:	(OPTIONAL, just preference)
|
|		Output Directory:					$(SolutionDir)\bin\$(ProjectName)-$(Platform)-$(Configuration)\
|		Intermediate Directory:				$(SolutionDir)\bin-int\$(ProjectName)-$(Platform)-$(Configuration)\
|
|	Debugging:
|
|		Working Directory:					$(OutDir)
|
|	C/C++ -> General:
|
|		Additional Incl. Directories:		$(SolutionDir)Libraries\SandFox\include;$(SolutionDir)Libraries\CHSL\include;%(AdditionalIncludeDirectories)
|			
|									=		$(SolutionDir)Libraries\SandFox\include
|											$(SolutionDir)Libraries\CHSL\include
|
|	Linker -> General:
|
|		Additional Lib. Directories:		$(SolutionDir)Libraries\SandFox\lib;$(SolutionDir)Libraries\CHSL\lib;%(AdditionalLibraryDirectories)
|
|									=		$(SolutionDir)Libraries\SandFox\lib
|											$(SolutionDir)Libraries\CHSL\lib
|
|	Linker -> Input:
|		
|		Additional Dependencies...
|
|			[ DEBUG CONFIGURATIONS ]:		CHSL64-d.lib;SandFox64-d.lib;%(AdditionalDependencies)
|			[ RELEASE CONFIGURATIONS ]:		CHSL64.lib;SandFox64.lib;%(AdditionalDependencies)
|
|	Linker -> System:
|
|		SubSystem:							Windows
|
|	Build Events -> Post-Build Event:
|
|		(Assuming that the output shader asset directory is OutDir/Assets/Shaders/..., and additional input assets are stored under Project/Assets/...)
|		Command Line...	
|
|			[ DEBUG CONFIGURATIONS ]:
------------------------------------------------------------------------------------------------------------------------------------
xcopy /y "$(SolutionDir)Libraries\SandFox\dll\SandFox$(PlatformArchitecture)-d.dll" "$(OutDir)"
xcopy /y /q /s /i "$(SolutionDir)Libraries\SandFox\shaders_debug" "$(OutDir)Assets\Shaders"
xcopy /y /q /s /i "$(ProjectDir)Assets\" "$(OutDir)Assets\"
------------------------------------------------------------------------------------------------------------------------------------
|
|			[ RELEASE CONFIGURATIONS ]:
------------------------------------------------------------------------------------------------------------------------------------
xcopy /y "$(SolutionDir)Libraries\SandFox\dll\SandFox$(PlatformArchitecture).dll" "$(OutDir)"
xcopy /y /q /s /i "$(SolutionDir)Libraries\SandFox\shaders" "$(OutDir)Assets\Shaders"
xcopy /y /q /s /i "$(ProjectDir)Assets\" "$(OutDir)Assets\"
------------------------------------------------------------------------------------------------------------------------------------
|
:



	---



New project example program:
------------------------------------------------------------------------------------------------------------------------------------



#include <SandFox.h>
#include <SandFox\Graphics.h>
#include <SandFox\Window.h>
#include <SandFox\Input.h>
#include <SandFox\Mesh.h>
#include <SandFox\MeshDrawable.h>
#include <SandFox\ImGuiHandler.h>
#include <SandFox\Debug.h>

#include <SandFox\ImGui\imgui.h>



#define TECHNIQUE			sx::GraphicsTechniqueImmediate

#define CAM_FOV				cs::c_pi * 0.5f
#define CAM_NEAR			0.01f
#define CAM_FAR				1000.0f

#define LIGHT_AMB_LEVEL		0.2f
#define LIGHT_AMB_COLOR		0xFFFFFF
#define LIGHT_DIR_LEVEL		1.8f
#define LIGHT_DIR_COLOR		0xFFFFF0



// Global debug and input system
sx::Debug g_debug;
sx::Input g_input;



int Run(HINSTANCE instance)
{
	// Create window
	sx::Window window;
	window.InitClass(instance);
	window.InitWindow(instance, 1280, 720, "Example", true);

	// Create graphics handler
	sx::Graphics graphics;
	sx::GraphicsTechnique technique = TECHNIQUE;
	graphics.Init(&window, L"Assets\\Shaders", technique, &g_debug);
	graphics.InitCamera({ -3, 3, -3 }, { 0.785f, 0.785f, 0 }, CAM_FOV, CAM_NEAR, CAM_FAR);

	// Initialize global input system
	g_input.LoadWindow(&window);
	g_input.MouseLocked(false);

	// Create GUI handler
	sx::ImGuiHandler imgui;
	imgui.Init(&graphics);

	// Setup lights
	cs::List<sx::LightID> lights;
	sx::LightHandler lightHandler;
	lightHandler.Init(sx::TextureQualityHighest, technique, cs::Color(LIGHT_AMB_COLOR), LIGHT_AMB_LEVEL);
	lights.Add(lightHandler.AddDirectionalShadowed({ 0.75f, 1.95f, 0.0f }, 50.0f, LIGHT_DIR_LEVEL, cs::Color(LIGHT_DIR_COLOR), 0.05f, 200.0f));

	// Load models
	sx::Mesh exampleMesh;
	exampleMesh.Load(L"Assets/Example.obj");
	sx::Prim::MeshDrawable exampleMeshDrawable(sx::Transform({}, { 0, cs::c_pi, 0 }), exampleMesh, false);

	// Setup draw queue
	sx::DrawQueue drawQueue;
	drawQueue.Init(false);

	window.Show();

	g_debug.CreateConsole();



	// Message pump and game loop

	cs::Timer timer;
	int exitCode = 0;

	for (int frame = 0;; frame++)
	{
		float dTime = timer.Lap();

		g_debug.ClearFrameTrace();

		if (const std::optional<int> optExitCode = window.ProcessMessages())
		{
			//DeInit();
			exitCode = *optExitCode;
			break;
		}

		g_input.CoreUpdateState();

		lightHandler.SetAmbient(cs::Color(LIGHT_AMB_COLOR), LIGHT_AMB_LEVEL);

		drawQueue.Clear();
		drawQueue.AddMain(&exampleMeshDrawable);

		for (int i = 0; i < lights.Size(); i++)
		{
			lightHandler.UpdateMap(lights[i]);
		}

		lightHandler.Update(&drawQueue);
		lightHandler.UpdateLightInfo();
		lightHandler.BindMaps();

		graphics.DrawFrame(&drawQueue);

		imgui.BeginDraw();
		ImGui::ShowDemoWindow();
		g_debug.DrawConsole();
		imgui.EndDraw();

		graphics.Present();
	}

	return exitCode;
}



int WINAPI WinMain(
	HINSTANCE	instance,
	HINSTANCE	prevInstance,
	char*		argv,
	int			showCmd)
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int exitCode = 0;

	g_debug.Init(false);

	try
	{
		exitCode = Run(instance);
	}
	catch (const cs::Exception& e)
	{
		g_input.MouseVisible(true);
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& e)
	{
		g_input.MouseVisible(true);
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		g_input.MouseVisible(true);
		MessageBoxA(nullptr, "No details", "Unknown Exception", MB_OK | MB_ICONERROR);
	}

	g_debug.DeInit(false);

	return exitCode;
}



------------------------------------------------------------------------------------------------------------------------------------
