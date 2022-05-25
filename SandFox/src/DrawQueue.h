#pragma once

#include "SandFoxCore.h"
#include "Drawable.h"

namespace SandFox
{

	class FOX_API DrawQueue
	{
	public:
		DrawQueue();
		~DrawQueue();

		void Init(bool autoClear);
		void DeInit();

		void AddPre(IDrawable* drawable);
		void AddMain(IDrawable* drawable);
		void AddPost(IDrawable* drawable);
		void Clear();

		void DrawPre();
		void DrawMain();
		void DrawPost();

	private:
		cs::List<IDrawable*> m_preDrawables;
		cs::List<IDrawable*> m_mainDrawables;
		cs::List<IDrawable*> m_postDrawables;

		bool m_autoClear;
	};

}
