#pragma once

#include <windows.h>

namespace Stamp
{
	static class FontProcessor
	{
	public:
		static HFONT CreateFontByHeight(HWND hWnd, LONG lHeight);
	};
}
