#include "core.h"

#include "Input.h"
#include "Window.h"

using namespace input::core;


bool g_mLocked;

Point g_mp, g_lastMp, g_mpDiff;

std::queue<PressEvent> g_keyboardQueue, g_mouseQueue;

std::bitset<UCHAR_MAX> g_keyPressed, g_keyDown, g_keyUp;
std::bitset<MAX_MOUSE_BUTTONS> g_mbPressed, g_mbDown, g_mbUp;



void input::core::updateState()
{
	g_mpDiff = g_mp - g_lastMp;

	g_lastMp = g_mLocked ? mouseLockedPosition() : g_mp;

	g_keyDown.reset();
	g_keyUp.reset();

	while (!g_keyboardQueue.empty())
	{
		PressEvent e = g_keyboardQueue.front();
		g_keyboardQueue.pop();

		switch (e.type)
		{
		case PressEventType::KeyUp:
			g_keyPressed[e.index] = false;
			g_keyUp[e.index] = true;
			break;

		case PressEventType::KeyDown:
			g_keyPressed[e.index] = true;
			g_keyDown[e.index] = true;
			break;
		}
	}

	while (!g_mouseQueue.empty())
	{
		PressEvent e = g_mouseQueue.front();
		g_mouseQueue.pop();

		switch (e.type)
		{
		case PressEventType::KeyUp:
			g_mbPressed[e.index] = false;
			g_mbUp[e.index] = true;
			break;

		case PressEventType::KeyDown:
			g_mbPressed[e.index] = true;
			g_mbDown[e.index] = true;
			break;
		}
	}

	if (g_mLocked)
	{
		moveMouseTo(mouseLockedPosition());
	}
}

void input::core::updateMousePosition(Point p)
{
	g_mp = p;
}

void input::core::queueKeyboard(PressEvent e)
{
	g_keyboardQueue.push(e);
}

void input::core::queueMouse(PressEvent e)
{
	g_mouseQueue.push(e);
}

void input::core::queueChar(wchar_t c)
{
}

bool input::keyPressed(byte keyCode)
{
	return g_keyPressed[keyCode];
}

bool input::keyDown(byte keyCode)
{
	return g_keyDown[keyCode];
}

bool input::keyUp(byte keyCode)
{
	return g_keyUp[keyCode];
}

bool input::mbPressed(byte keyCode)
{
	return g_mbPressed[keyCode];
}

bool input::mbDown(byte keyCode)
{
	return g_mbDown[keyCode];
}

bool input::mbUp(byte keyCode)
{
	return g_mbUp[keyCode];
}

void input::mouseVisible(bool show)
{
	ShowCursor(show);
}

void input::mouseLocked(bool locked)
{
	g_mLocked = locked;
}

bool input::getMouseLocked()
{
	return g_mLocked;
}

Point input::mouseLockedPosition()
{
	return { window::getW() / 2, window::getH() / 2 };
}

Point input::mousePositionDiff()
{
	return g_mpDiff;
}

Point input::mousePosition()
{
	return g_mp;
}

void input::moveMouseTo(Point p)
{
	POINT temp = { p.x, p.y };
	ClientToScreen(window::getHwnd(), &temp);
	SetCursorPos(temp.x, temp.y);
}
