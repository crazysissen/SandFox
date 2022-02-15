#pragma once

namespace input
{

	namespace core
	{
		
		const int MAX_MOUSE_BUTTONS = 10;

		enum class PressEventType
		{
			KeyUp, KeyDown, KeyNone
		};

		struct PressEvent
		{
			PressEventType type;
			byte index;
		};

		void updateState();

		void updateMousePosition(Point p);

		void queueKeyboard(PressEvent e);
		void queueMouse(PressEvent e);
		void queueChar(wchar_t c);

	}

	bool keyPressed(byte keyCode);
	bool keyDown(byte keyCode);
	bool keyUp(byte keyCode);

	bool mbPressed(byte keyCode);
	bool mbDown(byte keyCode);
	bool mbUp(byte keyCode);

	Point mousePositionDiff();
	Point mousePosition();
	void moveMouseTo(Point p);
	void mouseVisible(bool show);
	void mouseLocked(bool locked);
	bool getMouseLocked();
	Point mouseLockedPosition();

}

enum Key : byte
{
	KeyBack = 0x08,
	KeyTab,

	KeyEnter = 0x0D,

	KeyShift = 0x10,
	KeyControl, KeyAlt,

	KeyEsc = 0x1B,

	KeySpace = 0x20,
	KeyPgUp, KeyPgDown, KeyEnd, KeyHome, KeyLeft, KeyUp, KeyRight, KeyDown,

	KeyInsert = 0x2D,
	KeyDelete,

	KeyNum0 = 0x30,
	KeyNum1, KeyNum2, KeyNum3, KeyNum4, KeyNum5, KeyNum6, KeyNum7, KeyNum8, KeyNum9,

	KeyA = 0x41,
	KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, KeyK, KeyL, KeyM, KeyN, 
	KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT, KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ,

	KeyF1 = 0x70, 
	KeyF2, KeyF3, KeyF4, KeyF5, KeyF6, KeyF7, KeyF8, KeyF9, KeyF10, KeyF11, KeyF12,

	KeyLShift = 0xA0, 
	KeyRShift, KeyLControl, KeyRControl,

	KeyPlus = 0xBB,
	KeyComma, KeyMinus, KeyPeriod
};

enum MouseButton : byte
{
	MBLeft,
	MBRight,
	MBMiddle,
	MBX1,
	MBX2
};
