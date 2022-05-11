#include "pch.h"

#include <Windowsx.h>

#include "Window.h"
#include "Input.h"



int SandFox::Window::s_classIndexCounter = 0;



SandFox::Window::Window()
	:
	m_classIndex(-1),
	m_className(nullptr),
	m_inputWindow(false),

	m_handle(nullptr),
	m_instance(nullptr),
	m_externWndProc(nullptr),
	m_size(0, 0)
{
	FOX_TRACE("Constructing window.");
}

SandFox::Window::~Window()
{
	FOX_TRACE("Destructing window.");

	if (m_className != nullptr)
	{
		FOX_WARN("Window not manually deinitialized, proceeding with automatic deinitialization at destruction.");
		DeInitWindow();
	}
}



// Windows Procedure

LRESULT SandFox::Window::WindowsProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FOX_FTRACE_F("WndProc message (%x). WP: %lx, LP: %lx", msg, wParam, lParam);

	if (m_externWndProc)
	{
		if (m_externWndProc(hWnd, msg, wParam, lParam))
		{
			FOX_FTRACE("Extern WndProc override.");
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
	}

	if (m_inputWindow)
	{
		switch (msg)
		{
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
	}	

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



// Initialization and interaction logic

void SandFox::Window::InitClass(HINSTANCE instance, uint style, HICON icon, HCURSOR cursor)
{
	// Initialize window class
	WNDCLASSEX wndClass{ 0 };

	m_classIndex = s_classIndexCounter++;
	std::string className = (std::string(c_wndClassName) + std::to_string(m_classIndex));
	m_className = new char[className.size() + 1] { '\0' };
	std::memcpy(m_className, className.data(), className.size());

	FOX_TRACE_F("Initializing Window class: %s", m_className);

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = style;
	wndClass.lpfnWndProc = WndProcSetup;
	wndClass.lpszClassName = m_className;
	wndClass.hInstance = instance;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = icon;
	wndClass.hIconSm = nullptr;
	wndClass.hCursor = cursor;
	wndClass.hbrBackground = nullptr;

	if (!RegisterClassEx(&wndClass))
	{
		FOX_CRITICAL_F("Window class registration failed. HRES: %lx", (HRESULT)GetLastError());
		EXC_HRLAST();
	}
}

void SandFox::Window::InitClassDefault(HINSTANCE instance)
{
	InitClass(instance);
}

HWND SandFox::Window::InitWindow(HINSTANCE instance, int width, int height, cstr name, bool inputWindow)
{
	FOX_TRACE_F("Initializing Window: %s. Size: (%i x %i).", name, width, height);

	if (m_classIndex == -1)
	{
		InitClassDefault(instance);
	}

	m_instance = instance;
	m_inputWindow = inputWindow;

	m_size = { width, height };

	DWORD windowStyles = WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;

	RECT r;
	r.left = 50;
	r.top = 50;
	r.right = width + r.left;
	r.bottom = height + r.top;
	AdjustWindowRect(&r, windowStyles, FALSE);
	
	m_handle = CreateWindowEx(
		0,
		m_className,
		name,
		WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		r.right - r.left, r.bottom - r.top,
		nullptr, nullptr,
		m_instance,
		this
	);

	if (m_handle == nullptr)
	{
		FOX_CRITICAL_F("Window creation failed. HRES: %lx", (HRESULT)GetLastError());
		EXC_HRLAST();
	}

	return m_handle;
}

void SandFox::Window::DeInitWindow()
{
	FOX_TRACE_F("Deinitializing Window.");

	if (m_className != nullptr)
	{
		delete[] m_className;
		m_className = nullptr;
	}
}

void SandFox::Window::LoadPrioritizedWndProc(WNDPROC wndProc)
{
	m_externWndProc = wndProc;
}

void SandFox::Window::Show(int command)
{
	FOX_TRACE_F("Window Show command: %i", command);

	if (m_handle == nullptr)
		return;

	ShowWindow(m_handle, command);
}



// Message pump processor

std::optional<int> SandFox::Window::ProcessMessages()
{
	FOX_FTRACE_F("Call to ProcessMessages for Window.");

	static MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// Return optional int containing exit code if the message is a quit message
		if (msg.message == WM_QUIT)
		{
			FOX_TRACE_F("ProcessMessages caught WM_QUIT message.");
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
	return m_handle;
}

HINSTANCE__* SandFox::Window::GetHInstance()
{
	return m_instance;
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

LRESULT CALLBACK SandFox::Window::WndProcSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_NCCREATE:
	{
		FOX_TRACE_F("WndProcSetup caught WM_NCCREATE message, linking window procedure.");

		Window* wp = (Window*)(((CREATESTRUCTA*)lParam)->lpCreateParams); // Get window pointer from create struct
		SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProcPass);
		SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR)wp);
		return wp->WindowsProcedure(hWnd, msg, wParam, lParam);
	}

	default:
		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}
}

LRESULT CALLBACK SandFox::Window::WndProcPass(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Retrieve window pointer and call member function.
	Window* wp = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	return wp->WindowsProcedure(hWnd, msg, wParam, lParam);
}
