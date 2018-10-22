#pragma once

#include "StampDrawer.h"

namespace Stamp
{
	class RectangleStampDrawer : public StampDrawer
	{
	public:
		virtual BOOL Draw();
		RectangleStampDrawer(HWND hWnd, COLORREF crImageBackgroundColor);
	private:
		BOOL DrawBackgroundImage(HDC hDrawDC);

		int DrawTextByRectangle(HDC hDrawDC);
		int DrawTextByRectangleTopSide(HDC hDrawDC, int iFirstChar, int iLastChar);
		int DrawTextByRectangleRightSide(HDC hDrawDC, int iFirstChar, int iLastChar);
		int DrawTextByRectangleBottomSide(HDC hDrawDC, int iFirstChar, int iLastChar);
		int DrawTextByRectangleLeftSide(HDC hDrawDC, int iFirstChar, int iLastChar);
		BOOL DrawSymbol(HDC hDrawDC, TCHAR cSymbol, COORD cCenterPoint, LONG lPlaceholderWidth, WORD wAngle);

		void GetSymbolsCountForRectangleSides(int iTextLength, int aiLettersCount[]);
	};
}
