#pragma once

#include <windows.h>

namespace Stamp
{
	static class BitmapProcessor
	{
	public:
		static SIZE GetBitmapSize(HBITMAP hBitmap);
	};
}
