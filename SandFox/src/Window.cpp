#include "pch.h"

#include <Windowsx.h>

#include "Window.h"
#include "Input.h"
//#include "../Resources/resource.h"



SandFox::Window* SandFox::Window::s_window = nullptr;

SandFox::Window::Window()
	:
	m_hWnd(nullptr),
	m_hInst(nullptr),
	m_externWndProc(nullptr),
	m_size(0, 0)
{
	s_window = this;
}

SandFox::Window::~Window()
{
	s_window = nullptr;
}



// Windows Procedure

LRESULT WINAPI SandFox::Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (s_window != nullptr && s_window->m_externWndProc)
	{
		if (s_window->m_externWndProc(hWnd, msg, wParam, lParam))
		{
			return true;
		}
	}

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0x1);
		break;

	case WM_CREATE:
		break;

	case WM_KEYDOWN:
		Input::Get().CoreQueueKeyboard({ Input::PressEventType::KeyDown, (byte)wParam });
		break;

	case WM_KEYUP:
		Input::Get().CoreQueueKeyboard({ Input::PressEventType::KeyUp, (byte)wParam });
		break;

	case WM_CHAR:
		Input::Get().CoreQueueChar((wchar_t)wParam);
		break;

	case WM_LBUTTONDOWN:
		Input::Get().CoreQueueMouse({ Input::PressEventType::KeyDown, MBLeft });
		break;

	case WM_LBUTTONUP:
		Input::Get().CoreQueueMouse({ Input::PressEventType::KeyUp, MBLeft });
		break;

	case WM_RBUTTONDOWN:
		Input::Get().CoreQueueMouse({ Input::PressEventType::KeyDown, MBRight });
		break;

	case WM_RBUTTONUP:
		Input::Get().CoreQueueMouse({ Input::PressEventType::KeyUp, MBRight });
		break;

	case WM_MBUTTONDOWN:
		Input::Get().CoreQueueMouse({ Input::PressEventType::KeyDown, MBMiddle });
		break;

	case WM_MBUTTONUP:
		Input::Get().CoreQueueMouse({ Input::PressEventType::KeyUp, MBMiddle });
		break;

	case WM_XBUTTONDOWN:
		Input::Get().CoreQueueMouse({ Input::PressEventType::KeyDown, GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MBX1 : MBX2 });
		break;

	case WM_XBUTTONUP:
		Input::Get().CoreQueueMouse({ Input::PressEventType::KeyUp, GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MBX1 : MBX2 });
		break;

	case WM_MOUSEMOVE:
		cs::Point point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		//DBOUT("Mouse event: { " << point.x << ", " << point.y << "}")
		Input::Get().CoreUpdateMousePosition(point);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



// Initialization and interaction logic

void SandFox::Window::InitClass(HINSTANCE hInstance)
{
	SINGLEINIT("window class");

	m_hInst = hInstance;

	// Initialize window class
	WNDCLASSEX wndClass{ 0 };

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_OWNDC;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = c_wndClassName;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = nullptr;
	wndClass.hIconSm = nullptr;
	wndClass.hCursor = nullptr;
	wndClass.hbrBackground = nullptr;

	RegisterClassEx(&wndClass);
}

HWND SandFox::Window::InitWindow(int width, int height, cstr name)
{
	SINGLEINIT("window instance");

	m_size = { width, height };

	DWORD windowStyles = WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;

	RECT r;
	r.left = 50;
	r.top = 50;
	r.right = width + r.left;
	r.bottom = height + r.top;
	AdjustWindowRect(&r, windowStyles, FALSE);

	m_hWnd = CreateWindowEx(
		0,
		c_wndClassName,
		name,
		WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		r.right - r.left, r.bottom - r.top,
		nullptr, nullptr,
		m_hInst,
		nullptr
	);

	if (m_hWnd == nullptr)
	{
		EXC_HRLAST();
	}

	return m_hWnd;
}

void SandFox::Window::LoadPrioritizedWndProc(WNDPROC wndProc)
{
	m_externWndProc = wndProc;
}

void SandFox::Window::Show(int command)
{
	if (m_hWnd == nullptr)
		return;

	ShowWindow(m_hWnd, command);
}



// Message pump processor

std::optional<int> SandFox::Window::ProcessMessages()
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

HWND SandFox::Window::GetHwnd()
{
	return m_hWnd;
}

HINSTANCE__* SandFox::Window::GetHInstance()
{
	return m_hInst;
}

Point SandFox::Window::GetSize()
{
	return m_size;
}

int SandFox::Window::GetW()
{
	return m_size.x;
}

int SandFox::Window::GetH()
{
	return m_size.y;
}