#pragma once

#include "SandFoxCore.h"
#include <string>

namespace SandFox
{

	//class Window;

	enum DebugLevel
	{
		DebugLevelFrameTrace,	// Most verbose, information rarely useful, and expected to spam the console.
		DebugLevelTrace,		// Information very rarely useful on debug.
		DebugLevelDebug,		// Information judged to be useful on debug.
		DebugLevelWarn,			// Information related to potentially unintended behavior.
		DebugLevelError,		// Information related to erroneous behavior.
		DebugLevelCritical,		// Least verbose, information related to critical errors and system-breaking behavior.

		DebugLevelCommandTrace,	// Information following from commands.
		DebugLevelCommand,		// Information specifically triggered by user for debugging.

		DebugLevelNone,			// No output.

		DebugLevelCount		// <-- Keep last!
	};

	enum DebugCommandParamType
	{
		DebugCommandParamTypeNone,
		DebugCommandParamTypeInt,		// int
		DebugCommandParamTypeFloat,		// float
		DebugCommandParamTypeString,	// const char*, null-terminated

		DebygCommandParamTypeCount		// <-- Keep last!
	};

	typedef void (*DebugCommandCallback)(void* params, void* userData);

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

		void ClearFrameTrace();

		void WriteHelp();

		void WriteLog();

		static void PushMessage(DebugLevel level, const char* format, ...);
		static void PushMessage(const char* message, DebugLevel level = DebugLevelDebug);

		static void ExecuteCommand(const char* command);
		static void RegisterCommand(
			DebugCommandCallback callback,
			const DebugCommandParamType parameters[4],
			const std::string& identifier,
			const std::string& description = "",
			void* userData = nullptr);
		static void RegisterCommand(
			DebugCommandCallback callback,
			const std::string& identifier,
			const std::string& description = "",
			void* userData = nullptr);



	private:
		struct DebugItem
		{
			DebugLevel level;
			std::string text;
		};

		struct CommandItem
		{
			std::string identifier;
			std::string description;
			DebugCommandCallback callback;
			DebugCommandParamType parameters[4];
			int parameterCount;
			void* userData;
		};

	private:
		void PPushMessage(DebugLevel level, const char* format, va_list args);
		void PPushMessage(const char* message, DebugLevel level);

		void TryCommand();
		void RegisterDefaultCommands();

		static void TryPrintDescription(const CommandItem& command, DebugLevel level);
		static void CommandHelp(void* params, void* userData);
		static void CommandLog(void* params, void* userData);

	private:
		static constexpr int c_tempBufferSize = 2048;
		static constexpr int c_inputBufferSize = 128;
		static constexpr int c_dataBufferSize = 144;
		static constexpr char c_commandChar = '>';
		static Debug* s_debug;

		DebugLevel m_debugLevel;
		DebugLevel m_displayLevel;
		bool m_initialized;

		cs::List<DebugItem> m_items;

		cs::Vec4 m_textColors[DebugLevelCount];

		void* m_commands;
		cs::List<int> m_frameTraceIndices;

		//Window* m_consoleWindow;
		//std::thread m_consoleThread;
		char* m_tempBuffer;
		char* m_inputBuffer;
		byte* m_dataBuffer;
		bool m_scrollToBottom;
		bool m_showCommandLine;
	};

}

#ifndef FOX_NO_LOG

#define FOX_FTRACE(str) SandFox::Debug::PushMessage((str), SandFox::DebugLevelFrameTrace);
#define FOX_FTRACE_F(str, ...) SandFox::Debug::PushMessage(SandFox::DebugLevelFrameTrace, (str), __VA_ARGS__);

#define FOX_TRACE(str) SandFox::Debug::PushMessage((str), SandFox::DebugLevelTrace);
#define FOX_TRACE_F(str, ...) SandFox::Debug::PushMessage(SandFox::DebugLevelTrace, (str), __VA_ARGS__);

#define FOX_LOG(str) SandFox::Debug::PushMessage((str), SandFox::DebugLevelDebug);
#define FOX_LOG_F(str, ...) SandFox::Debug::PushMessage(SandFox::DebugLevelDebug, (str), __VA_ARGS__);

#define FOX_WARN(str) SandFox::Debug::PushMessage((str), SandFox::DebugLevelWarn);
#define FOX_WARN_F(str, ...) SandFox::Debug::PushMessage(SandFox::DebugLevelWarn, (str), __VA_ARGS__);

#define FOX_ERROR(str) SandFox::Debug::PushMessage((str), SandFox::DebugLevelError);
#define FOX_ERROR_F(str, ...) SandFox::Debug::PushMessage(SandFox::DebugLevelError, (str), __VA_ARGS__);

#define FOX_CRITICAL(str) SandFox::Debug::PushMessage((str), SandFox::DebugLevelCritical);
#define FOX_CRITICAL_F(str, ...) SandFox::Debug::PushMessage(SandFox::DebugLevelCritical, (str), __VA_ARGS__);

#define FOX_COMMAND(cmd) SandFox::Debug::ExecuteCommand((cmd));

#else

#define FOX_FTRACE(str)
#define FOX_FTRACE_F(str, ...)

#define FOX_TRACE(str)
#define FOX_TRACE_F(str, ...)

#define FOX_LOG(str)
#define FOX_LOG_F(str, ...)

#define FOX_WARN(str)
#define FOX_WARN_F(str, ...)

#define FOX_ERROR(str)
#define FOX_ERROR_F(str, ...)

#define FOX_CRITICAL(str) l)
#define FOX_CRITICAL_F(str, ...)

#define FOX_COMMAND(cmd)

#endif
