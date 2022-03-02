#pragma once

#include "SandFoxCore.h"
#include "Texture.h"
#include "IBindable.h"

#include <string>

namespace SandFox
{
	namespace Bind
	{

		class FOX_API TextureBindable : public IBindable
		{
		public:
			TextureBindable(unsigned int registerIndex = 10u);
			TextureBindable(const Texture& texture, unsigned int registerIndex = 10u);
			TextureBindable(std::wstring name, unsigned int registerIndex = 10u);
			TextureBindable(const TextureBindable& copy);

			void Bind() override;

		private:
			Texture m_texture;
			unsigned int m_registerIndex;
		};

	}
}