#pragma once

#include <optional>

namespace window
{

	const int
		c_width = 1920,
		c_height = 1080;

	// Window class name, not window name
	constexpr cstr wndClassName = "vworldClassDX11";

	// Initialize window class for application
	void initClass(HINSTANCE hInstance);
	// Register the window on Windows API side
	HWND initWindow(int width, int height, cstr name);
	// Loads external WndProc which has priority over default proc
	void loadPrioritizedWndProc(WNDPROC wndProc);
	// Show the window to the user
	void showWindow(int command = SW_SHOW);

	// Process the current message stack and optionally return quit message
	std::optional<int> processMessages();

	// Get handle to window
	HWND getHwnd();
	// Get application instance
	HINSTANCE getHInstance();

	// Get window raster dimensions
	Point getSize();
	int getW();
	int getH();

	// The windows procedure
	LRESULT WINAPI wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};