#pragma once

#include "StampDrawer.h"

namespace Stamp
{
	class SemicircleStampDrawer : public StampDrawer
	{
	public:
		virtual BOOL Draw();
		SemicircleStampDrawer(HWND hWnd, COLORREF crImageBackgroundColor);
	private:
		BOOL DrawBackgroundImage(HDC hDrawDC);

		int DrawBottomSymbols(HDC hDrawDC, int iStartSymbol);
		int DrawArcSymbols(HDC hDrawDC, int iArcSymbolsCount);
		BOOL DrawSymbol(HDC hDrawDC, TCHAR cSymbol, COORD cCenterPoint, LONG lPlaceholderWidth, DOUBLE dAngle);
		int GetBottomSymbolsCount();
		COORD GetArcSymbolCenterCoordinates(DOUBLE dAngle);
	};
}
