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
	m_debugLevel(DebugLevelFrameTrace),
	m_displayLevel(DebugLevelDebug),
	m_initialized(false),
	//m_consoleWindow(nullptr),
	m_scrollToBottom(false),
	m_showCommandLine(true),

	m_tempBuffer(nullptr),
	m_inputBuffer(nullptr),

	m_textColors
	{
		{ 0.4f, 0.4f, 0.4f, 1.0f },
		{ 0.6f, 0.6f, 0.6f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 0.4f, 1.0f },
		{ 1.0f, 0.6f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },

		{ 0.2f, 0.5f, 0.2f, 1.0f },
		{ 0.4f, 1.0f, 0.4f, 1.0f },

		{}
	}
{
}

SandFox::Debug::~Debug()
{
	//if (m_consoleWindow != nullptr)
	//{
	//	DestroyConsole();
	//}

	if (m_initialized)
	{
		DeInit(true);
	}
}

void SandFox::Debug::Init()
{
	s_debug = this;

	m_initialized = true;
	m_tempBuffer = new char[c_tempBufferSize + 1] { '\0' };
	m_inputBuffer = new char[c_inputBufferSize + 1]{ '\0' };
	m_dataBuffer = new byte[c_dataBufferSize + 1] { 0 };

	m_commands = new std::unordered_map<std::string, CommandItem>();

	RegisterDefaultCommands();
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
	delete[] m_dataBuffer;

	delete m_commands;

	if (s_debug == this)
	{
		s_debug = nullptr;
	}
}



int ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args);

void SandFox::Debug::PushMessage(DebugLevel level, const char* format, ...)
{
	if (!s_debug || s_debug->m_debugLevel > level)
	{
		return;
	}

	va_list args;
	va_start(args, format);
	s_debug->PPushMessage(level, format, args);
	va_end(args);
}
void SandFox::Debug::PushMessage(const char* message, DebugLevel level)
{
	if (!s_debug || s_debug->m_debugLevel > level)
	{
		return;
	}

	s_debug->PPushMessage(message, level);
}

void SandFox::Debug::PPushMessage(DebugLevel level, const char* format, va_list args)
{
	const char* end = m_tempBuffer + ImFormatStringV(m_tempBuffer, c_tempBufferSize, format, args);
	m_items.Add(DebugItem{ level, std::string(m_tempBuffer, (int)(end - m_tempBuffer)) });

	if (level == DebugLevelFrameTrace)
	{
		m_frameTraceIndices.Add(m_items.Size() - 1);
	}

	TryCommand();
}

void SandFox::Debug::PPushMessage(const char* message, DebugLevel level)
{
	m_items.Add(DebugItem{ level, std::string(message) });

	if (level == DebugLevelFrameTrace)
	{
		m_frameTraceIndices.Add(m_items.Size() - 1);
	}

	TryCommand();
}

void SandFox::Debug::TryCommand()
{
	const DebugItem& item = m_items.Back();

	if (item.text[0] == c_commandChar)
	{
		std::stringstream full(&item.text[1]);
		std::string id; 
		full >> id;

		std::unordered_map<std::string, CommandItem>* c = (std::unordered_map<std::string, CommandItem>*)m_commands;

		if (c->contains(id))
		{
			int paramsCount = 0;
			std::string params[4];
			while (!full.eof())
			{
				full >> params[paramsCount];
				paramsCount++;
			}

			const CommandItem& command = (*c)[id];

			if (paramsCount > command.parameterCount)
			{
				PushMessage(DebugLevelWarn, "Excess parameters given, command \"%s\" accepts a maximum of %i parameters.", id.c_str(), command.parameterCount);
				TryPrintDescription(command, DebugLevelWarn);
				return;
			}
			else if (paramsCount < command.parameterCount)
			{
				PushMessage(DebugLevelError, "Insufficient parameters given, command \"%s\" requires %i parameters.", id.c_str(), command.parameterCount);
				TryPrintDescription(command, DebugLevelError);
				return;
			}
			else
			{
				byte* current = m_dataBuffer;
				byte* end = m_dataBuffer + c_dataBufferSize - 1;

				try
				{
					for (int i = 0; i < paramsCount; i++)
					{
						switch (command.parameters[i])
						{
						case DebugCommandParamTypeInt:
						{
							int* p = (int*)current;
							*p = std::stoi(params[i]);
							current[sizeof(int)] = 0;
							current += sizeof(int);
						}
						break;

						case DebugCommandParamTypeFloat:
						{
							float* p = (float*)current;
							*p = std::stof(params[i]);
							current[sizeof(float)] = 0;
							current += sizeof(float);
						}
						break;

						case DebugCommandParamTypeString:
						{
							size_t size = params[i].size();
							end -= (size + 1);
							std::memcpy(end, params[i].data(), size);
							end[size] = 0;

							const char** p = (cstr*)current;
							*p = (cstr)end;
							current += sizeof(cstr);
						}
						break;

						default:	// Includes TypeNone
							break;
						}
					}
				}
				catch (...)
				{
					PushMessage(DebugLevelError, "Error parsing parameters.", id.c_str(), command.parameterCount);
					TryPrintDescription(command, DebugLevelError);
					return;
				}

				*current = 0;
			}

			// Redirect to the callback function.
			command.callback(m_dataBuffer, command.userData);
		}
		else
		{
			PushMessage(DebugLevelWarn, "Command \"%s\" not recognized. Type \"%c help\" for a list of registered commands.", id.c_str(), c_commandChar);
		}
	}
}

void SandFox::Debug::RegisterDefaultCommands()
{
	RegisterCommand(CommandHelp, "help", "Display all registered commands.", this);
	RegisterCommand(CommandLog, "log", "Log debug stream to file.", this);
}

void SandFox::Debug::TryPrintDescription(const CommandItem& command, DebugLevel level)
{
	static std::string typeStrings[DebygCommandParamTypeCount] =
	{
		"None",
		"Int",
		"Float",
		"String"
	};

	std::string message = command.identifier;

	if (command.parameterCount > 0)
	{
		message += " [";

		for (int i = 0; i < command.parameterCount; i++)
		{
			if (i > 0)
			{
				message += ", ";
			}

			message += typeStrings[command.parameters[i]].c_str();
		}

		message += "]";
	}

	if (command.description != "")
	{
		message += ": \"" + command.description + "\"";
	}

	PushMessage(message.c_str(), DebugLevelCommandTrace);
}

void SandFox::Debug::CommandHelp(void* params, void* userData)
{
	Debug* d = (Debug*)userData;
	d->WriteHelp();
}

void SandFox::Debug::CommandLog(void* params, void* userData)
{
	Debug* d = (Debug*)userData;
	d->WriteLog();
}

void SandFox::Debug::CreateConsole()
{
}

void SandFox::Debug::DestroyConsole()
{
}

void SandFox::Debug::DrawConsole()
{
	if (ImGui::Begin("Debug Console", nullptr, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Config"))
			{
				static const char* strings[7] = { "Frame Trace", "Trace", "Debug", "Warn", "Error", "Critical", "Commands" };
				if (ImGui::ListBox(
					"Debug Level",
					(int*)&m_debugLevel,
					strings,
					7))
				{
					PushMessage((DebugLevel)std::max(m_debugLevel, DebugLevelWarn), "Debug level set to %s.", strings[m_debugLevel]);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Display"))
			{
				static const char* strings[7] = { "Frame Trace (!)", "Trace", "Debug", "Warn", "Error", "Critical", "Commands" };
				ImGui::ListBox(
					"Filter",
					(int*)&m_displayLevel,
					strings,
					7
				);
				ImGui::Separator();
				ImGui::Checkbox("Command Line", &m_showCommandLine);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		float consoleHeight = 0.0f;
		if (m_showCommandLine)
		{
			consoleHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		}
		 
		if (ImGui::BeginChild("Console", ImVec2(0, -consoleHeight)))
		{
			ImGui::PushTextWrapPos();

			for (int i = 0; i < m_items.Size(); i++)
			{
				DebugItem& item = m_items[i];

				if (item.level < m_displayLevel)
				{
					continue;
				}

				if (item.level != DebugLevelCommandTrace)
				{
					ImGui::Spacing();
				}

				switch (item.level)
				{
				case DebugLevelDebug:
					ImGui::TextUnformatted(item.text.data());
					break;

				//case DebugLevelCommand:
				//	ImGui::PushStyleColor(ImGuiCol_Text, m_textColors[DebugLevelCommand]);
				//	ImGui::TextUnformatted(item.text.data());
				//	ImGui::PopStyleColor();
				//	break;

				default:
					ImGui::PushStyleColor(ImGuiCol_Text, *(ImVec4*)&m_textColors[item.level]);
					ImGui::TextUnformatted(item.text.data());
					ImGui::PopStyleColor();
					break;
				}
			}

			ImGui::PopTextWrapPos();

			if (m_scrollToBottom)
			{
				ImGui::SetScrollHereY(1.0f);
				m_scrollToBottom = false;
			}

			ImGui::Spacing();

			ImGui::EndChild();
		}

		if (m_showCommandLine)
		{
			ImGui::Separator();

			bool buttonPress = ImGui::Button("Execute");
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			bool enterPress = ImGui::InputText("Input", m_inputBuffer, c_inputBufferSize, ImGuiInputTextFlags_EnterReturnsTrue);

			if ((buttonPress || enterPress))
			{
				if (m_inputBuffer[0] != '\0')
				{
					PushMessage(DebugLevelCommand, "%c %s", c_commandChar, m_inputBuffer);
					m_scrollToBottom = true;
				}

				std::memset(m_inputBuffer, '\0', std::strlen(m_inputBuffer));
			}
			ImGui::PopItemWidth();

			ImGui::SetItemDefaultFocus();
			if (buttonPress || enterPress)
			{
				ImGui::SetKeyboardFocusHere(-1);
			}
		}
	}
	ImGui::End();
}

void SandFox::Debug::ClearFrameTrace()
{
	if (m_frameTraceIndices.Size() > 0)
	{
		m_items.MassRemove(m_frameTraceIndices.Data(), m_frameTraceIndices.Size());
		m_frameTraceIndices.Clear(false);
	}
}

void SandFox::Debug::WriteHelp()
{
	for (std::pair<std::string, CommandItem> p : *((std::unordered_map<std::string, CommandItem>*)m_commands))
	{
		//PushMessage(p.second.identifier.c_str(), DebugLevelCommandTrace);
		TryPrintDescription(p.second, DebugLevelCommandTrace);
	}
}

void SandFox::Debug::WriteLog()
{
}

void SandFox::Debug::ExecuteCommand(const char* command)
{
	if (command[0] == c_commandChar)
	{
		PushMessage(command, DebugLevelCommand);
	}
	else
	{
		PushMessage(DebugLevelCommand, "%c %s", c_commandChar, command);
	}
}

void SandFox::Debug::RegisterCommand(DebugCommandCallback callback, const DebugCommandParamType parameters[4], const std::string& identifier, const std::string& description, void* userData)
{
	if (!s_debug)
	{
		return;
	}

	int paramCount = 0;
	for (; paramCount < 4 && parameters[paramCount] != DebugCommandParamTypeNone; paramCount++);

	(*((std::unordered_map<std::string, CommandItem>*)s_debug->m_commands))[identifier] =
	{
		identifier,
		description,
		callback,
		{ parameters[0], parameters[1], parameters[2], parameters[3] },
		paramCount,
		userData
	};
}

void SandFox::Debug::RegisterCommand(DebugCommandCallback callback, const std::string& identifier, const std::string& description, void* userData)
{
	DebugCommandParamType noParams[4] = { DebugCommandParamTypeNone };
	RegisterCommand(callback, noParams, identifier, description, userData);
}
