#pragma once

#include "Graphics.h"

class IBindable
{
public:
	virtual ~IBindable() = default;

	virtual void Bind() = 0;
};