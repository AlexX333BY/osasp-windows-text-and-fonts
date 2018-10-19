#pragma once

#include <windows.h>

namespace Stamp
{
	class BitmapProcessor
	{
	public:
		static SIZE GetBitmapSize(HBITMAP hBitmap);
	};
}
