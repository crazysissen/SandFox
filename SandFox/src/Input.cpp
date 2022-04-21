#include "pch.h"

#include "Input.h"
#include "Window.h"



SandFox::Input* SandFox::Input::s_input = nullptr;

SandFox::Input::Input()
	:
	m_window(nullptr),

	m_mLocked(false),
	m_lastMp(0, 0),
	m_mp(0, 0),
	m_mpDiff(0, 0),

	m_keyboardQueue(),
	m_mouseQueue(),
	m_charQueue(),

	m_frameChars(new wchar_t[MIN_FRAME_CHAR_CAPACITY]),
	m_frameCharCount(0),
	m_frameCharCapacity(MIN_FRAME_CHAR_CAPACITY),

	m_keyPressed(),
	m_keyDown(),
	m_keyUp(),
	m_mbPressed(),
	m_mbDown(),
	m_mbUp()
{
	s_input = this;
}

SandFox::Input::Input(Window* window)
	:
	Input()
{
	m_window = window;
}

SandFox::Input::~Input()
{
	delete[] m_frameChars;

	s_input = nullptr;

	ShowCursor(true);
}

void SandFox::Input::LoadWindow(Window* window)
{
	m_window = window;
}

Point SandFox::Input::WindowLockPosition()
{
	return { m_window->GetW() / 2, m_window->GetH() / 2 };
}

void SandFox::Input::MoveMouseTo(Point p)
{
	POINT temp = { p.x, p.y };
	ClientToScreen(m_window->GetHwnd(), &temp);
	SetCursorPos(temp.x, temp.y);
	m_mp = p;
}

void SandFox::Input::CoreUpdateState()
{
	m_mpDiff = m_mp - m_lastMp;
	m_lastMp = m_mLocked ? WindowLockPosition() : m_mp;

	if (m_mpDiff != cs::Point(0, 0))
	{
		//DBOUT("Mouse movement: { " << m_mpDiff.x << ", " << m_mpDiff.y << " }");
	}

	m_keyDown.reset();
	m_keyUp.reset();

	while (!m_keyboardQueue.Empty())
	{
		PressEvent e = m_keyboardQueue.Pop();

		switch (e.type)
		{
		case PressEventType::KeyUp:
			m_keyPressed[e.index] = false;
			m_keyUp[e.index] = true;
			break;

		case PressEventType::KeyDown:
			m_keyPressed[e.index] = true;
			m_keyDown[e.index] = true;
			break;
		}
	}

	while (!m_mouseQueue.Empty())
	{
		PressEvent e = m_mouseQueue.Pop();

		switch (e.type)
		{
		case PressEventType::KeyUp:
			m_mbPressed[e.index] = false;
			m_mbUp[e.index] = true;
			break;

		case PressEventType::KeyDown:
			m_mbPressed[e.index] = true;
			m_mbDown[e.index] = true;
			break;
		}
	}

	if (m_charQueue.Size() > m_frameCharCapacity)
	{
		do
		{
			m_frameCharCapacity *= 2;
		} 
		while (m_charQueue.Size() > m_frameCharCapacity);

		delete[] m_frameChars;
		m_frameChars = new wchar_t[m_frameCharCapacity];
	}

	m_frameCharCount = (int)m_charQueue.Size();

	for (int i = 0; !m_charQueue.Empty(); i++)
	{
		wchar_t c = m_charQueue.Pop();

		m_frameChars[i] = c;
	}


	if (m_mLocked)
	{
		MoveMouseTo(WindowLockPosition());
	}
}

void SandFox::Input::CoreUpdateMousePosition(Point p)
{
	//DBOUT("Mouse position change: { " << p.x << ", " << p.y << " }");
	m_mp = p;
}

void SandFox::Input::CoreQueueKeyboard(PressEvent e)
{
	m_keyboardQueue.Push(e);
}

void SandFox::Input::CoreQueueMouse(PressEvent e)
{
	m_mouseQueue.Push(e);
}

void SandFox::Input::CoreQueueChar(wchar_t c)
{
	m_charQueue.Push(c);
}

bool SandFox::Input::KeyPressed(byte keyCode)
{
	return s_input->m_keyPressed[keyCode];
}

bool SandFox::Input::KeyDown(byte keyCode)
{
	return s_input->m_keyDown[keyCode];
}

bool SandFox::Input::KeyUp(byte keyCode)
{
	return s_input->m_keyUp[keyCode];
}

bool SandFox::Input::MbPressed(byte keyCode)
{
	return s_input->m_mbPressed[keyCode];
}

bool SandFox::Input::MbDown(byte keyCode)
{
	return s_input->m_mbDown[keyCode];
}

bool SandFox::Input::MbUp(byte keyCode)
{
	return s_input->m_mbUp[keyCode];
}

void SandFox::Input::MouseVisible(bool show)
{
	ShowCursor(show);
}

void SandFox::Input::MouseLocked(bool locked)
{
	if (s_input->m_window == nullptr)
	{
		return;
	}

	s_input->m_mLocked = locked;
}

bool SandFox::Input::GetMouseLocked()
{
	return s_input->m_mLocked;
}

SandFox::Input& SandFox::Input::Get()
{
	return *s_input;
}

Point SandFox::Input::MousePositionDiff()
{
	return s_input->m_mpDiff;
}

Point SandFox::Input::MousePosition()
{
	return s_input->m_mp;
}
