#include "core.h"

#include <Windowsx.h>

#include "Window.h"
#include "Input.h"
//#include "../Resources/resource.h"



HWND g_hWnd				= nullptr;
HINSTANCE g_hInst			= nullptr;

WNDPROC g_externWndProc	= nullptr;

Point g_size;



// Windows Procedure

LRESULT WINAPI window::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_externWndProc)
	{
		if (g_externWndProc(hWnd, msg, wParam, lParam))
		{
			return true;
		}
	}

	using namespace Input::Core;

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0x1);
		break;

	case WM_CREATE:
		//graphics::initGraphics();
		break;

	case WM_KEYDOWN:
		queueKeyboard({ PressEventType::KeyDown, (byte)wParam });
		break;

	case WM_KEYUP:
		queueKeyboard({ PressEventType::KeyUp, (byte)wParam });
		break;

	case WM_CHAR:
		queueChar((wchar_t)wParam);
		break;

	case WM_LBUTTONDOWN:
		queueMouse({ PressEventType::KeyDown, MBLeft });
		break;

	case WM_LBUTTONUP:
		queueMouse({ PressEventType::KeyUp, MBLeft });
		break;

	case WM_RBUTTONDOWN:
		queueMouse({ PressEventType::KeyDown, MBRight });
		break;

	case WM_RBUTTONUP:
		queueMouse({ PressEventType::KeyUp, MBRight });
		break;

	case WM_MBUTTONDOWN:
		queueMouse({ PressEventType::KeyDown, MBMiddle });
		break;

	case WM_MBUTTONUP:
		queueMouse({ PressEventType::KeyUp, MBMiddle });
		break;

	case WM_XBUTTONDOWN:
		queueMouse({ PressEventType::KeyDown, GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MBX1 : MBX2 });
		break;

	case WM_XBUTTONUP:
		queueMouse({ PressEventType::KeyUp, GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MBX1 : MBX2 });
		break;

	case WM_MOUSEMOVE:
		//Point mPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		//if (input::getMouseLocked() && input::mouseLockedPosition() == mPos)
		//{
		//	break;
		//}
		updateMousePosition({ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



// Initialization and interaction logic

void window::initClass(HINSTANCE hInstance)
{
	SINGLEINIT("window class");

	g_hInst = hInstance;

	// Initialize window class
	WNDCLASSEX wndClass{ 0 };

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_OWNDC;
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = wndClassName;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = nullptr;
	wndClass.hIconSm = nullptr;
	wndClass.hCursor = nullptr;
	wndClass.hbrBackground = nullptr;

	RegisterClassEx(&wndClass);
}

HWND window::initWindow(int width, int height, cstr name)
{
	SINGLEINIT("window instance");

	g_size = { width, height };

	DWORD windowStyles = WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;

	RECT r;
	r.left = 50;
	r.top = 50;
	r.right = width + r.left;
	r.bottom = height + r.top;
	AdjustWindowRect(&r, windowStyles, FALSE);

	g_hWnd = CreateWindowEx(
		0,
		wndClassName,
		name,
		WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		r.right - r.left, r.bottom - r.top,
		nullptr, nullptr,
		g_hInst,
		nullptr
	);

	if (g_hWnd == nullptr)
	{
		EXC_HRLAST();
	}

	return g_hWnd;
}

void window::loadPrioritizedWndProc(WNDPROC wndProc)
{
	g_externWndProc = wndProc;
}

void window::showWindow(int command)
{
	if (g_hWnd == nullptr)
		return;

	ShowWindow(g_hWnd, command);
}



// Message pump processor

std::optional<int> window::processMessages()
{
	static MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// Return optional int containing exit code if the message is a quit message
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;
		}

		// Create auxillary messages and then pass to WndProc
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}



// Getters

HWND window::getHwnd()
{
	return g_hWnd;
}

HINSTANCE__* window::getHInstance()
{
	return g_hInst;
}

Point window::getSize()
{
	return g_size;
}

int window::getW()
{
	return g_size.x;
}

int window::getH()
{
	return g_size.y;
}
