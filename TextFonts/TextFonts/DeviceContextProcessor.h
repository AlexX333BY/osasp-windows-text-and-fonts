#pragma once

#include <windows.h>

namespace Stamp
{
	class DeviceContextProcessor
	{
	public:
		static int FillRectangle(HDC hDC, COLORREF crColor, RECT rRectToFill);
		static BOOL FillEllipse(HDC hDC, COLORREF crFillColor, RECT rEllipseRect);
	};
}
