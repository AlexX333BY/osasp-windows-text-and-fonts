#pragma once

#include <windows.h>

namespace Stamp
{
	class WindowProcessor
	{
	public:
		static LONG GetWindowFontHeight(HWND hWnd);
		static SIZE GetWindowSize(HWND hWnd);
		static int FillWindowWithColor(HWND hWnd, COLORREF crColor);
		static COLORREF GetDefaultBackgroundColor();
	};
}
