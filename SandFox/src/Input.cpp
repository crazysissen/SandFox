#include "pch.h"

#include "Input.h"
#include "Window.h"



SandFox::Input* SandFox::Input::s_input = nullptr;

SandFox::Input::Input()
	:
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

SandFox::Input::~Input()
{
	delete[] m_frameChars;

	s_input = nullptr;
}

void SandFox::Input::CoreUpdateState()
{
	m_mpDiff = m_mp - m_lastMp;

	m_lastMp = m_mLocked ? MouseLockedPosition() : m_mp;

	m_keyDown.reset();
	m_keyUp.reset();

	while (!m_keyboardQueue.empty())
	{
		PressEvent e = m_keyboardQueue.front();
		m_keyboardQueue.pop();

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

	while (!m_mouseQueue.empty())
	{
		PressEvent e = m_mouseQueue.front();
		m_mouseQueue.pop();

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

	if (m_charQueue.size() > m_frameCharCapacity)
	{
		do
		{
			m_frameCharCapacity *= 2;
		} 
		while (m_charQueue.size() > m_frameCharCapacity);

		delete[] m_frameChars;
		m_frameChars = new wchar_t[m_frameCharCapacity];
	}

	m_frameCharCount = (int)m_charQueue.size();

	for (int i = 0; !m_charQueue.empty(); i++)
	{
		wchar_t c = m_charQueue.front();
		m_charQueue.pop();

		m_frameChars[i] = c;
	}


	if (m_mLocked)
	{
		MoveMouseTo(MouseLockedPosition());
	}
}

void SandFox::Input::CoreUpdateMousePosition(Point p)
{
	m_mp = p;
}

void SandFox::Input::CoreQueueKeyboard(PressEvent e)
{
	m_keyboardQueue.push(e);
}

void SandFox::Input::CoreQueueMouse(PressEvent e)
{
	m_mouseQueue.push(e);
}

void SandFox::Input::CoreQueueChar(wchar_t c)
{
	m_charQueue.push(c);
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
	s_input->m_mLocked = locked;
}

bool SandFox::Input::GetMouseLocked()
{
	return s_input->m_mLocked;
}

Point SandFox::Input::MouseLockedPosition()
{
	return { Window::GetW() / 2, Window::GetH() / 2 };
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

void SandFox::Input::MoveMouseTo(Point p)
{
	POINT temp = { p.x, p.y };
	ClientToScreen(Window::GetHwnd(), &temp);
	SetCursorPos(temp.x, temp.y);
}
