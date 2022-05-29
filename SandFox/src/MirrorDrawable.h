#pragma once

#include "Drawable.h"
#include "Shader.h"
#include "Cubemap.h"

namespace SandFox
{

	class FOX_API MirrorDrawable : public IDrawable
	{
	public:
		MirrorDrawable();
		~MirrorDrawable();

		void Load(IDrawable* target, Cubemap* cubemap, bool tesselation);

		void Draw() override;

	private:
		IDrawable* m_target;
		Cubemap* m_cubemap;
		Shader* m_shader;
		bool m_tesselation;
	};

}

