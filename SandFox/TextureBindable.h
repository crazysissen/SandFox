#pragma once

#include "Texture.h"
#include "IBindable.h"

namespace ibind
{

	class TextureBindable : public Texture, public IBindable
	{
	public:
		TextureBindable(const TextureBindable& copy);
		TextureBindable(uint registerIndex = 10u);
		TextureBindable(std::wstring name, uint registerIndex = 10u);

		void Bind() override;

	private:
		uint m_registerIndex;
	};

}