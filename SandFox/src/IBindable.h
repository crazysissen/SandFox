#pragma once

#include "Graphics.h"

namespace SandFox
{

	class IBindable
	{
	public:
		virtual ~IBindable() = default;

		virtual void Bind() = 0;
	};

}