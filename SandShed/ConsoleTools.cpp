#include "ConsoleTools.h"

#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <conio.h>



#pragma region Writes

void CWrite(const char* text)
{
	std::cout << text;
}

void CWriteLine()
{
	std::cout << std::endl;
}

void CWriteLine(const char* text)
{
	std::cout << text << std::endl;
}

void CWriteChar(char ch)
{
	std::cout << ch;
}

void CWriteInt(int i)
{
	std::cout << i;
}

#pragma endregion



void CSetColor(Color foreground, Color background)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, foreground + (background * 16));
}

char CGetChar(bool write, bool writeEndl)
{
	char in = _getch();

	if (write)
	{
		CWriteChar(in);
	}
	if (writeEndl)
	{
		CWriteLine();
	}

	return in;
}

int CGetDigit()
{
	while (true)
	{
		char in = CGetChar(true, true);

		if (in >= 0x30 && in < 0x3A)
		{
			return in - 0x30;
		}

		CWrite("Invalid, please try again: ");
	}
}

int CGetDigit(int min, int max)
{
	while (true)
	{
		int in = CGetDigit();

		if (in >= min && in < max)
		{
			return in;
		}

		CWrite("Invalid, please try again: ");
	}
}

bool CGetYN(const char* prompt)
{
	CWrite(prompt);

	while (true)
	{
		char in = CGetChar(false, false);

		if (in == 'y' || in == 'Y')
		{
			CWriteLine("Yes");
			return true;
		}

		if (in == 'n' || in == 'N')
		{
			CWriteLine("No");
			return false;
		}

		CWriteChar(in);
		CWrite("\nInvalid, please try again: ");
	}
}

std::string CIn()
{
	std::string input;
	std::getline(std::cin, input);
	//std::cin >> input;

	return input;
}

