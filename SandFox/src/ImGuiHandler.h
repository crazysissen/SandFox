#pragma once

#include "SandFoxCore.h"
#include "Graphics.h"


namespace SandFox
{

	enum ImGuiStyleBasic
	{
		ImGuiStyleDark,
		ImGuiStyleLight,
		ImGuiStyleClassic
	};

	class FOX_API ImGuiHandler
	{
	public:
		ImGuiHandler();
		ImGuiHandler(Graphics* graphics, ImGuiStyleBasic style = ImGuiStyleDark);
		~ImGuiHandler();

		void Init(Graphics* graphics, ImGuiStyleBasic style = ImGuiStyleDark);
		void DeInit();

		void BeginDraw();
		void EndDraw();

		void SetStyle(ImGuiStyleBasic style);

	private:
		bool m_initialized;
		Graphics* m_graphics;
	};

}