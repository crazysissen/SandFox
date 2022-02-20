#include "core.h"

#include "Input.h"
#include "Window.h"

using namespace SandFox::Input::Core;


bool g_mLocked;

Point g_mp, g_lastMp, g_mpDiff;

std::queue<PressEvent> g_keyboardQueue, g_mouseQueue;

std::bitset<UCHAR_MAX> g_keyPressed, g_keyDown, g_keyUp;
std::bitset<MAX_MOUSE_BUTTONS> g_mbPressed, g_mbDown, g_mbUp;



void SandFox::Input::Core::updateState()
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

void SandFox::Input::Core::updateMousePosition(Point p)
{
	g_mp = p;
}

void SandFox::Input::Core::queueKeyboard(PressEvent e)
{
	g_keyboardQueue.push(e);
}

void SandFox::Input::Core::queueMouse(PressEvent e)
{
	g_mouseQueue.push(e);
}

void SandFox::Input::Core::queueChar(wchar_t c)
{
}

bool SandFox::Input::keyPressed(byte keyCode)
{
	return g_keyPressed[keyCode];
}

bool SandFox::Input::keyDown(byte keyCode)
{
	return g_keyDown[keyCode];
}

bool SandFox::Input::keyUp(byte keyCode)
{
	return g_keyUp[keyCode];
}

bool SandFox::Input::mbPressed(byte keyCode)
{
	return g_mbPressed[keyCode];
}

bool SandFox::Input::mbDown(byte keyCode)
{
	return g_mbDown[keyCode];
}

bool SandFox::Input::mbUp(byte keyCode)
{
	return g_mbUp[keyCode];
}

void SandFox::Input::mouseVisible(bool show)
{
	ShowCursor(show);
}

void SandFox::Input::mouseLocked(bool locked)
{
	g_mLocked = locked;
}

bool SandFox::Input::getMouseLocked()
{
	return g_mLocked;
}

Point SandFox::Input::mouseLockedPosition()
{
	return { window::getW() / 2, window::getH() / 2 };
}

Point SandFox::Input::mousePositionDiff()
{
	return g_mpDiff;
}

Point SandFox::Input::mousePosition()
{
	return g_mp;
}

void SandFox::Input::moveMouseTo(Point p)
{
	POINT temp = { p.x, p.y };
	ClientToScreen(window::getHwnd(), &temp);
	SetCursorPos(temp.x, temp.y);
}
