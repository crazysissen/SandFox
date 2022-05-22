#pragma once

#include "SandFoxCore.h"
#include "Drawable.h"

namespace SandFox
{

	class DrawQueue
	{
	public:
		DrawQueue();
		~DrawQueue();

		void Init(bool autoClear);
		void DeInit();

		void AddMain(IDrawable* drawable);
		void AddPost(IDrawable* drawable);
		void Clear();

		void DrawMain();
		void DrawPost();

	private:
		cs::List<IDrawable*> m_mainDrawables;
		cs::List<IDrawable*> m_postDrawables;

		bool m_autoClear;
	};

}
