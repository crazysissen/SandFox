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
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(m_graphics->GetWindow()->GetHwnd());
	ImGui_ImplDX11_Init(m_graphics->GetDevice().Get(), m_graphics->GetContext().Get());

	// Set the imgui input-overriding wndproc
	m_graphics->GetWindow()->LoadPrioritizedWndProc(ImGui_ImplWin32_WndProcHandler);

	SetStyle(style);

	Graphics::Get().InitImgui();
}

SandFox::ImGuiHandler::~ImGuiHandler()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	
	ImGui::DestroyContext();
}

void SandFox::ImGuiHandler::BeginDraw()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void SandFox::ImGuiHandler::EndDraw()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void SandFox::ImGuiHandler::SetStyle(ImGuiStyleBasic style)
{
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
