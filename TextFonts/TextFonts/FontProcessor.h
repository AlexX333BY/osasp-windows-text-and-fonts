#pragma once

#include <windows.h>

namespace Stamp
{
	class FontProcessor
	{
	public:
		static HFONT CreateFontByHeight(HWND hWnd, LONG lHeight);
	};
}
