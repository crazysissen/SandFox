#pragma once

#include "SandFoxCore.h"

#include <queue>
#include <bitset>

namespace SandFox
{

	class FOX_API Input
	{
	public:

		enum class PressEventType
		{
			KeyUp, KeyDown, KeyNone
		};

		struct PressEvent
		{
			PressEventType type;
			byte index;
		};



		// Input functions

		static bool KeyPressed(byte keyCode);
		static bool KeyDown(byte keyCode);
		static bool KeyUp(byte keyCode);
		
		static bool MbPressed(byte keyCode);
		static bool MbDown(byte keyCode);
		static bool MbUp(byte keyCode);
		
		static Point MousePositionDiff();
		static Point MousePosition();
		static void MoveMouseTo(Point p);
		static void MouseVisible(bool show);
		static void MouseLocked(bool locked);
		static bool GetMouseLocked();
		static Point MouseLockedPosition();

		static Input& Get();



		// Core

		Input();
		virtual ~Input();

		void CoreUpdateState();
		void CoreUpdateMousePosition(Point p);

		void CoreQueueKeyboard(PressEvent e);
		void CoreQueueMouse(PressEvent e);
		void CoreQueueChar(wchar_t c);



	private:
		static constexpr int MAX_MOUSE_BUTTONS = 8;
		static constexpr int MIN_FRAME_CHAR_CAPACITY = 8;

		static Input* s_input;

		bool m_mLocked;

		Point m_lastMp;
		Point m_mp;
		Point m_mpDiff;

		std::queue<PressEvent> m_keyboardQueue;
		std::queue<PressEvent> m_mouseQueue;
		std::queue<wchar_t> m_charQueue;

		wchar_t* m_frameChars;
		int m_frameCharCount;
		int m_frameCharCapacity;

		std::bitset<0xFF> m_keyPressed;
		std::bitset<0xFF> m_keyDown;
		std::bitset<0xFF> m_keyUp;
		std::bitset<MAX_MOUSE_BUTTONS> m_mbPressed;
		std::bitset<MAX_MOUSE_BUTTONS> m_mbDown; 
		std::bitset<MAX_MOUSE_BUTTONS> m_mbUp;
	};

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

}