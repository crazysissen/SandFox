#pragma once

#include "SandFoxCore.h"
#include <optional>

namespace SandFox
{

	class FOX_API Window
	{
	public:
		Window();
		~Window();

		// Initialize window class for application
		void InitClass(HINSTANCE hInstance);
		// Register the window on Windows API side
		HWND InitWindow(int width, int height, cstr name);
		// Loads external WndProc which has priority over default proc
		void LoadPrioritizedWndProc(WNDPROC wndProc);
		// Show the window to the user
		void Show(int command = SW_SHOW);

		// Process the current message stack and optionally return quit message
		std::optional<int> ProcessMessages();

		HWND GetHwnd();
		HINSTANCE GetHInstance();

		// Get window raster dimensions
		Point GetSize();
		int GetW();
		int GetH();

		// The windows procedure
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	private:
		// Defaults
		static constexpr int c_dWidth = 1280;
		static constexpr int c_dHeight = 720;

		// Window class name, not window name
		static constexpr cstr c_wndClassName = "SandFoxClassDX11";

		static Window* s_window;

		HWND m_hWnd;
		HINSTANCE m_hInst;
		WNDPROC m_externWndProc;
		Point m_size;
	};

}
