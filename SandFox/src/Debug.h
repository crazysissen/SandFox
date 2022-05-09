#pragma once

#include "SandFoxCore.h"
#include <thread>
#include <string>
#include "ImGui/imgui.h"

namespace SandFox
{

	class Window;

	enum DebugLevel
	{
		DebugLevelTrace,	// Most verbose, information very rarely useful on debug.
		DebugLevelDebug,	// Information judged to be useful on debug.
		DebugLevelWarn,		// Information related to potentially unintended behavior.
		DebugLevelError,	// Information related to erroneous behavior.
		DebugLevelCritical,	// Least verbose, information related to critical errors and system-breaking behavior.
		DebugLevelNone,		// No output.
		DebugLevelCommand,	// Information specifically triggered by user for debugging.

		DebugLevelCount	// <-- Keep Last!
	};

	class FOX_API Debug
	{
	public:
		Debug();
		~Debug();

		void Init();
		void DeInit(bool writeLog = false);

		void CreateConsole();
		void DestroyConsole();

		void DrawConsole();

		void WriteLog();

		static void PushMessage(DebugLevel level, const char* format, ...);
		//static void PushMessage(DebugLevel level, const char* message);

	private:
		void PPushMessage(DebugLevel level, const char* format, va_list args);
		void PPushMessage(DebugLevel level, const char* message);

		static int InputCallback(ImGuiInputTextCallbackData* data);

	private:
		struct DebugItem
		{
			DebugLevel level;
			std::string text;
		};

		static constexpr int c_tempBufferSize = 2048;
		static constexpr int c_inputBufferSize = 128;
		static Debug* s_debug;

		DebugLevel m_debugLevel;
		bool m_initialized;

		cs::List<DebugItem> m_items;

		ImVec4 m_textColors[DebugLevelCount];

		Window* m_consoleWindow;
		std::thread m_consoleThread;
		char* m_tempBuffer;
		char* m_inputBuffer;
	};


}