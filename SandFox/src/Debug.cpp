#include "pch.h"

#include "Debug.h"
#include "Window.h"
#include "ImGui/imgui.h"

#include <format>
#include <dxgidebug.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Include dxguid debug library for the project
#pragma comment(lib, "dxguid.lib")



SandFox::Debug* SandFox::Debug::s_debug = nullptr;

SandFox::Debug::Debug()
	:
	m_debugLevel(),
	m_initialized(false),
	m_consoleWindow(nullptr),

	m_textColors
	{
		{ 0.6f, 0.6f, 0.6f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 0.4f, 1.0f },
		{ 1.0f, 0.6f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{},
		{ 0.4f, 1.0f, 0.4f, 1.0f }
	}
{
}

SandFox::Debug::~Debug()
{
	if (m_consoleWindow != nullptr)
	{
		DestroyConsole();
	}

	if (m_initialized)
	{
		DeInit(true);
	}
}

void SandFox::Debug::Init()
{
	m_initialized = true;
	m_tempBuffer = new char[c_tempBufferSize + 1] { '\0' };
	m_inputBuffer = new char[c_inputBufferSize + 1] { '\0' };

	s_debug = this;
}

void SandFox::Debug::DeInit(bool writeLog)
{
	if (writeLog)
	{
		WriteLog();
	}

	m_initialized = false;
	delete[] m_tempBuffer;
	delete[] m_inputBuffer;

	if (s_debug == this)
	{
		s_debug = nullptr;
	}
}



int ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args);

void SandFox::Debug::PushMessage(DebugLevel level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	s_debug->PPushMessage(level, format, args);
	va_end(args);
}/*

void SandFox::Debug::PushMessage(DebugLevel level, const char* message)
{
	s_debug->PPushMessage(level, message);
}*/

void SandFox::Debug::PPushMessage(DebugLevel level, const char* format, va_list args)
{
	const char* end = m_tempBuffer + ImFormatStringV(m_tempBuffer, c_tempBufferSize, format, args);
	m_items.Add(DebugItem{ level, std::string(m_tempBuffer, (int)(end - m_tempBuffer)) });
}

void SandFox::Debug::PPushMessage(DebugLevel level, const char* message)
{
	m_items.Add(DebugItem{ level, std::string(message) });
}

int SandFox::Debug::InputCallback(ImGuiInputTextCallbackData* data)
{
	return 0;
}

void SandFox::Debug::CreateConsole()
{
	//m_consoleWindow = new Window();

	//HINSTANCE instance = GetModuleHandle(NULL);
	//m_consoleWindow->InitClass(instance);
	//m_consoleWindow->InitWindow(instance, 400, 400, "Debug Console", false);



	//m_consoleWindow->Show();
}

void SandFox::Debug::DestroyConsole()
{
	//delete m_consoleWindow;
}

void SandFox::Debug::DrawConsole()
{
	if (ImGui::Begin("Debug Console"))
	{
		float consoleHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

		if (ImGui::BeginChild("Console", ImVec2(0, -consoleHeight)))
		{
			ImGui::PushTextWrapPos();

			for (int i = 0; i < m_items.Size(); i++)
			{
				DebugItem& item = m_items[i];

				if (item.level != DebugLevelDebug)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, m_textColors[item.level]);
					ImGui::TextUnformatted(item.text.data());
					ImGui::PopStyleColor();
				}
				else
				{
					ImGui::TextUnformatted(item.text.data());
				}
			}

			ImGui::PopTextWrapPos();

			ImGui::EndChild();
		}

		ImGuiInputTextFlags inputTextFlags =
			ImGuiInputTextFlags_CallbackHistory | 
			ImGuiInputTextFlags_CallbackCharFilter | 
			ImGuiInputTextFlags_CallbackCompletion |
			ImGuiInputTextFlags_EnterReturnsTrue | 
			ImGuiInputTextFlags_CallbackAlways;

		if (ImGui::InputText("Input", m_inputBuffer, c_inputBufferSize, inputTextFlags, InputCallback, this))
		{
			PushMessage(DebugLevelCommand, m_inputBuffer);
		}

		ImGui::End();
	}
}

void SandFox::Debug::WriteLog()
{

}
