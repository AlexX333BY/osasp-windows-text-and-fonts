#pragma once

#include "StampDrawer.h"
#include <windows.h>

namespace Stamp
{
	class EllipseStampDrawer : public StampDrawer
	{
	public:
		virtual BOOL Draw();
		EllipseStampDrawer(HWND hWnd, COLORREF crImageBackgroundColor);
	private:
		BOOL DrawBackgroundImage(HDC hDrawDC);
		COORD GetSymbolCenterCoordinates(DOUBLE dAngle);
		BOOL DrawSymbol(HDC hDrawDC, TCHAR cSymbol, COORD cCenterPoint, LONG lPlaceholderWidth, DOUBLE dAngle);
	};
}
