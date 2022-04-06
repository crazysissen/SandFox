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
		ImGuiHandler(Graphics* graphics, ImGuiStyleBasic style = ImGuiStyleDark);
		~ImGuiHandler();

		void BeginDraw();
		void EndDraw();

		void SetStyle(ImGuiStyleBasic style);

	private:
		Graphics* m_graphics;
	};

}