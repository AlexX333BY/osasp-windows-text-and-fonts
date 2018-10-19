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
		BOOL DrawBackgroundImage();

		int DrawTextByRectangle();
		int DrawTextByRectangleTopSide(int iFirstChar, int iLastChar);
		int DrawTextByRectangleRightSide(int iFirstChar, int iLastChar);
		int DrawTextByRectangleBottomSide(int iFirstChar, int iLastChar);
		int DrawTextByRectangleLeftSide(int iFirstChar, int iLastChar);
		BOOL DrawSymbol(TCHAR cSymbol, COORD cCenterPoint, LONG lPlaceholderWidth, WORD wAngle);

		void GetSymbolsCountForRectangleSides(int iTextLength, int aiLettersCount[]);
	};
}
