#include "pch.h"

#include "ImGuiHandler.h"
#include "Graphics.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



SandFox::ImGuiHandler::ImGuiHandler(Graphics* graphics, ImGuiStyleBasic style)
	:
	m_graphics(graphics)
{
	FOX_TRACE("Constructing ImGuiHandler.");
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	FOX_TRACE("Initializing ImGui Win32 and DX11 implementation.");
	ImGui_ImplWin32_Init(m_graphics->GetWindow()->GetHwnd());
	ImGui_ImplDX11_Init(m_graphics->GetDevice().Get(), m_graphics->GetContext().Get());

	//ImGuiPlatformIO& pio = ImGui::GetPlatformIO();
	//pio.Renderer_CreateWindow = imgui_impldx11_creat

	// Set the imgui input-overriding wndproc
	m_graphics->GetWindow()->LoadPrioritizedWndProc(ImGui_ImplWin32_WndProcHandler);

	SetStyle(style);

	Graphics::Get().InitImgui();
}

SandFox::ImGuiHandler::~ImGuiHandler()
{
	FOX_TRACE("Deconstructing ImGuiHandler.");
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	
	ImGui::DestroyContext();
}

void SandFox::ImGuiHandler::BeginDraw()
{
	FOX_FTRACE("Begin new ImGui frame.");
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void SandFox::ImGuiHandler::EndDraw()
{
	FOX_FTRACE("Render finished ImGui frame.");
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		FOX_FTRACE("Update ImGui platform windows.");
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void SandFox::ImGuiHandler::SetStyle(ImGuiStyleBasic style)
{
	FOX_TRACE("ImGui style changed.");

	switch (style)
	{
		case SandFox::ImGuiStyleClassic:
			ImGui::StyleColorsClassic();
			break;

		case SandFox::ImGuiStyleDark:
			ImGui::StyleColorsDark();
			break;

		case SandFox::ImGuiStyleLight:
			ImGui::StyleColorsLight();
			break;
	}
}
