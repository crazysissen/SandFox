#pragma once

#include <string>

enum Color : char
{
	ColorBlack,
	ColorDarkBlue,
	ColorDarkGreen,
	ColorDarkCyan,
	ColorDarkRed,
	ColorDarkMagenta,
	ColorDarkYellow,
	ColorLightGrey,
	ColorDarkGrey,
	ColorBlue,
	ColorGreen,
	ColorCyan,
	ColorRed,
	ColorMagenta,
	ColorYellow,
	ColorWhite
};

void CWrite(const char* text);
void CWriteLine(const char* text);
void CWriteLine();
void CWriteChar(char ch);
void CWriteInt(int i);

void CSetColor(Color foreground, Color background = ColorBlack);

char CGetChar(bool write, bool writeEndl);
int CGetDigit();
int CGetDigit(int min, int max);
bool CGetYN(const char* prompt);

std::string CIn();