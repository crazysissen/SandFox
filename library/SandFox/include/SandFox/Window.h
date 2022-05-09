#pragma once

#include "SandFoxCore.h"
#include <optional>

namespace SandFox
{

	// HOW TO USE
	// ----------
	// 1. Create instance. 
	// 2. [optional] Init class manually, default class is used otherwise. 
	// 3. Initialize window.
	// 

	class FOX_API Window
	{
	public:
		Window();
		~Window();

		// Optional. Default style is CS_OWNDC
		void InitClass(HINSTANCE instance, uint style = 0x20, HICON icon = nullptr, HCURSOR cursor = nullptr);

		// Register the window on Windows API side
		HWND InitWindow(HINSTANCE instance, int width, int height, cstr name, bool inputWindow);

		// Loads external WndProc which has priority over default proc
		void LoadPrioritizedWndProc(WNDPROC wndProc);

		// Show the window to the user
		void Show(int command = SW_SHOW);

		// Process the current message stack and optionally return quit message
		std::optional<int> ProcessMessages();

		// Windows procedure member function
		LRESULT WindowsProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND GetHwnd();
		HINSTANCE GetHInstance();

		// Get window raster dimensions
		Point GetSize();
		int GetW();
		int GetH();

	private:
		// Initialize window class for this window
		void InitClassDefault(HINSTANCE instance);
		// Initial windows procedure to configure instance link
		static LRESULT CALLBACK WndProcSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		// Windows procedure passthrough to instance
		static LRESULT CALLBACK WndProcPass(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	private:
		// Defaults
		static constexpr int c_dWidth = 1280;
		static constexpr int c_dHeight = 720;

		// Window class name, not window name
		static constexpr cstr c_wndClassName = "SandFoxClass_";

		static int s_classIndexCounter;

		int m_classIndex;
		char* m_className;
		bool m_inputWindow;

		HWND m_handle;
		HINSTANCE m_instance;
		WNDPROC m_externWndProc;
		Point m_size;
	};

}
