#pragma once

namespace SandFox
{

	class IBindable
	{
	public:
		virtual ~IBindable() = default;

		virtual void Bind() = 0;
	};

}