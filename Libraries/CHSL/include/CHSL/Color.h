#pragma once

#include "CHSLTypedef.h"

namespace cs
{
	
	class Color
	{
	public:
		struct Bytes
		{
			byte r, g, b, a;
		};

		float r, g, b;

		Color();
		Color(float red, float green, float blue);
		Color(uint colorHex); // 6 byte hexadecimal!

		void Mutate(float ammount, cs::Random& random, bool monochrome = true);

		virtual Bytes GetBytes();

		Color& operator*=(const float& colorM);
	};

	class ColorA : public Color
	{
	public:
		float a;

		ColorA();
		ColorA(const Color& c);
		ColorA(float red, float green, float blue, float alpha);
		ColorA(uint colorHex); // 8 byte hexadecimal!

		Color::Bytes GetBytes() override;
	};

}