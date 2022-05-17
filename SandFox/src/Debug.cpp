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

void SandFox::Debug::Init(bool captureStreams)
{
	s_debug = this;

	// m_mutex = new std::mutex();

	m_initialized = true;
	m_tempBuffer = new char[c_tempBufferSize + 1] { '\0' };
	m_inputBuffer = new char[c_inputBufferSize + 1]{ '\0' };
	m_dataBuffer = new byte[c_dataBufferSize + 1] { 0 };

	m_commands = new std::unordered_map<std::string, CommandItem>();

	if (captureStreams)
	{
		CaptureStream(&std::cerr, DebugLevelError);
		CaptureStream(&std::cout, DebugLevelDebug);
		CaptureStream(&std::clog, DebugLevelTrace);

		//CaptureDebugOutput();
	}

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

	delete (std::unordered_map<std::string, CommandItem>*)m_commands;

	ReleaseStreams();

	// delete m_mutex;

	if (s_debug == this)
	{
		s_debug = nullptr;
	}
}

void SandFox::Debug::CaptureStream(std::ios* stream, DebugLevel level)
{
	m_streams.Add({ {}, stream->rdbuf(), stream });

	StreamRedirect& d = m_streams.Back();
	d.buffer.SetLevel(level);
	stream->rdbuf(&d.buffer);
}

void SandFox::Debug::ReleaseStreams()
{
	for (StreamRedirect& s : m_streams)
	{
		s.stream->rdbuf(s.streamBuffer);
	}

	m_streams.Clear();

	if (m_debugOutputCaptor.Initialized())
	{
		m_debugOutputCaptor.DeInit();
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

	// m_mutex->lock();

	m_items.Add(DebugItem{ level, std::string(m_tempBuffer, (int)(end - m_tempBuffer)) });

	if (level == DebugLevelFrameTrace)
	{
		m_frameTraceIndices.Add(m_items.Size() - 1);
	}

	TryCommand();

	// m_mutex->unlock();
}

void SandFox::Debug::PPushMessage(const char* message, DebugLevel level)
{
	//m_mutex->lock();

	m_items.Add(DebugItem{ level, std::string(message) });

	if (level == DebugLevelFrameTrace)
	{
		m_frameTraceIndices.Add(m_items.Size() - 1);
	}

	TryCommand();

	//m_mutex->unlock();
}

void SandFox::Debug::CaptureDebugOutput()
{
	bool success = m_debugOutputCaptor.Init(DebugLevelDebug);

	if (success)
	{
		PushMessage("Debug output stream captured successfuly.", DebugLevelDebug);

		m_debugOutputCaptor.Detach();
	}
	else
	{
		PushMessage("Debug output stream not captured.", DebugLevelError);
	}
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
			PushMessage(DebugLevelWarn, "Command \"%s\" not recognized. Type \"help\" for a list of registered commands.", id.c_str(), c_commandChar);
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



// streambuf

SandFox::Debug::DebugStreambuf::DebugStreambuf()
	:
	std::streambuf(),
	m_level(DebugLevelDebug)
{
}

SandFox::Debug::DebugStreambuf::~DebugStreambuf()
{
}

void SandFox::Debug::DebugStreambuf::SetLevel(DebugLevel level)
{
	m_level = level;
}

void SandFox::Debug::DebugStreambuf::Push()
{
	m_buffer.Add('\0');
	PushMessage(m_buffer.Data(), m_level);
	m_buffer.Clear(false);
}

int SandFox::Debug::DebugStreambuf::overflow(int c)
{
	int result = EOF;

	if (c < 0 || c == '\n' || c == '\r')
	{
		result = sync();
		Push();
	}
	else if (c >= 0 && c <= UCHAR_MAX)
	{
		result = c;
		m_buffer.Add(c);
	}
	else
	{
		result = c;
		PushMessage(DebugLevelWarn, "Invalid value [%x] pushed to DebugStreambuf.", c);
	}

	return result;
}

int SandFox::Debug::DebugStreambuf::sync()
{
	return 0;
}



// Debug output captor

SandFox::Debug::DebugOutputCaptor::DebugOutputCaptor()
	:
	m_initialized(false),

	m_mutexHandle(nullptr),
	m_bufferReadyEvent(nullptr),
	m_dataReadyEvent(nullptr),
	m_bufferHandle(nullptr),

	m_buffer(nullptr),
	m_thread(nullptr)
{
}

SandFox::Debug::DebugOutputCaptor::~DebugOutputCaptor()
{
}

bool SandFox::Debug::DebugOutputCaptor::Init(DebugLevel level)
{
	PushMessage("Initializing DebugOutputCaptor.", DebugLevelDebug);

	m_level = level;

	m_mutexHandle = nullptr;
	m_bufferReadyEvent = nullptr;
	m_dataReadyEvent = nullptr;
	m_bufferHandle = nullptr;
	m_buffer = nullptr;

	// Mutex
	m_mutexHandle = OpenMutexA(SYNCHRONIZE, FALSE, "DBWinMutex");
	if (m_mutexHandle == nullptr)
	{
		PushMessage(DebugLevelError, "Failed to open DBWIN mutex. Error: %s", cs::ExceptionWindows::TranslateHRESULT(GetLastError()).c_str());
		DeInit();
		return false;
	}

	// Buffer ready event
	m_bufferReadyEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, "DBWIN_BUFFER_READY");
	if (m_bufferReadyEvent == nullptr)
	{
		PushMessage("Failed to open BufferReady event, attempting to create.", DebugLevelWarn);

		m_bufferReadyEvent = CreateEventA(NULL, FALSE, TRUE, "DBWIN_BUFFER_READY");
		if (m_bufferReadyEvent == nullptr)
		{
			PushMessage(DebugLevelError, "Failed to create BufferReady event. Error: %s", cs::ExceptionWindows::TranslateHRESULT(GetLastError()).c_str());
			DeInit();
			return false;
		}
	}

	// Data ready event
	m_dataReadyEvent = OpenEventA(SYNCHRONIZE, FALSE, "DBWIN_DATA_READY");
	if (m_bufferReadyEvent == nullptr)
	{
		PushMessage("Failed to open DataReady event, attempting to create.", DebugLevelWarn);

		m_bufferReadyEvent = CreateEventA(NULL, FALSE, FALSE, "DBWIN_DATA_READY");
		if (m_bufferReadyEvent == nullptr)
		{
			PushMessage(DebugLevelError, "Failed to create DataReady event. Error: %s", cs::ExceptionWindows::TranslateHRESULT(GetLastError()).c_str());
			DeInit();
			return false;
		}
	}

	// Buffer
	m_bufferHandle = OpenFileMappingA(FILE_MAP_READ, FALSE, "DBWIN_BUFFER");
	if (m_bufferHandle == nullptr)
	{
		PushMessage("Failed to open DBWIN buffer, attempting to create.", DebugLevelWarn);

		m_bufferHandle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DBWINBuffer), "DBWIN_BUFFER");
		if (m_bufferHandle == nullptr)
		{
			PushMessage(DebugLevelError, "Failed to create DBWIN buffer. Error: %s", cs::ExceptionWindows::TranslateHRESULT(GetLastError()).c_str());
			DeInit();
			return false;
		}
	}

	m_buffer = (DBWINBuffer*)MapViewOfFile(m_bufferHandle, SECTION_MAP_READ, 0, 0, 0);

	if (m_buffer == nullptr)
	{
		PushMessage(DebugLevelError, "Failed to map DBWIN buffer to local pointer. Error: %s", cs::ExceptionWindows::TranslateHRESULT(GetLastError()).c_str());
		DeInit();
		return false;
	}

	m_initialized = true;

	return true;
}

void SandFox::Debug::DebugOutputCaptor::DeInit()
{
	PushMessage("Deinitializing DebugOutputCaptor.", DebugLevelTrace);

	if (m_thread != nullptr)
	{
		Terminate();
	}

	if (m_mutexHandle != nullptr)
	{
		CloseHandle(m_mutexHandle);
		m_mutexHandle = nullptr;
	}

	if (m_bufferReadyEvent != nullptr)
	{
		CloseHandle(m_bufferReadyEvent);
		m_bufferReadyEvent = nullptr;
	}

	if (m_dataReadyEvent != nullptr)
	{
		CloseHandle(m_dataReadyEvent);
		m_dataReadyEvent = nullptr;
	}

	if (m_bufferHandle != nullptr)
	{
		CloseHandle(m_bufferHandle);
		UnmapViewOfFile(m_buffer);
		m_bufferHandle = nullptr;
	}

	m_buffer = nullptr;
}

void SandFox::Debug::DebugOutputCaptor::Detach()
{
	PushMessage("Creating and detaching DebugOutputCaptor thread.", DebugLevelTrace);

	m_thread = new std::thread(Run, this);
	m_thread->detach();
}

void SandFox::Debug::DebugOutputCaptor::Terminate()
{
	PushMessage("Terminating DebugOutputCaptor thread.", DebugLevelTrace);
	delete m_thread;
}

bool SandFox::Debug::DebugOutputCaptor::Initialized()
{
	return m_initialized;
}

void SandFox::Debug::DebugOutputCaptor::Run(DebugOutputCaptor* captor)
{
	PushMessage("DebugOutputCaptor thread running.", DebugLevelTrace);

	for (;;)
	{
		DWORD returnValue = WaitForSingleObject(captor->m_dataReadyEvent, c_debutOutputTimeout);

		if (returnValue == WAIT_OBJECT_0)
		{
			PushMessage(captor->m_buffer->data, captor->m_level);
			SetEvent(captor->m_bufferReadyEvent);
		}
	}
}
