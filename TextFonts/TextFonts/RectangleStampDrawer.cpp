#include "RectangleStampDrawer.h"
#include "BitmapProcessor.h"
#include "FontProcessor.h"
#include "StringProcessor.h"
#include "WindowProcessor.h"
#include "XformCreator.h"

namespace Stamp
{
	BOOL RectangleStampDrawer::Draw()
	{
		WindowProcessor::FillWindowWithColor(m_hWnd, m_crImageBackgroundColor);
		BOOL bResult = TRUE;
		if (m_hBackgroundImage != NULL)
		{
			bResult &= DrawBackgroundImage();
		}
		return bResult && DrawTextByRectangle();
	}

	BOOL RectangleStampDrawer::DrawBackgroundImage()
	{
		if (m_hBackgroundImage == NULL)
		{
			return FALSE;
		}
		
		SIZE sBackgroundSize = BitmapProcessor::GetBitmapSize(m_hBackgroundImage);
		HDC hWndDC = GetDC(m_hWnd);
		HDC hBackgroundDC = CreateCompatibleDC(hWndDC);
		HGDIOBJ hOldObject = SelectObject(hBackgroundDC, m_hBackgroundImage);

		BOOL bResult = StretchBlt(hWndDC, m_cStampCoordinates.X, m_cStampCoordinates.Y, m_sStampSize.cx, m_sStampSize.cy, hBackgroundDC, 0, 0, sBackgroundSize.cx, sBackgroundSize.cy, SRCCOPY);

		SelectObject(hBackgroundDC, hOldObject);
		DeleteDC(hBackgroundDC);
		ReleaseDC(m_hWnd, hWndDC);
		return bResult;
	}

	int RectangleStampDrawer::DrawTextByRectangle()
	{
		int aiLettersOnSide[4];
		GetSymbolsCountForRectangleSides(lstrlen(m_lpsText), aiLettersOnSide);
		int iFirstLetter, iLastLetter;
		int iResult = 0;

		if (aiLettersOnSide[0] > 0)
		{
			iFirstLetter = 0;
			iLastLetter = aiLettersOnSide[0] - 1;
			iResult += DrawTextByRectangleTopSide(iFirstLetter, iLastLetter);
		}

		if (aiLettersOnSide[1] > 0)
		{
			iFirstLetter = aiLettersOnSide[0];
			iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] - 1;
			iResult += DrawTextByRectangleRightSide(iFirstLetter, iLastLetter);
		}

		if (aiLettersOnSide[2] > 0)
		{
			iFirstLetter = aiLettersOnSide[0] + aiLettersOnSide[1];
			iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2] - 1;
			iResult += DrawTextByRectangleBottomSide(iFirstLetter, iLastLetter);
		}

		if (aiLettersOnSide[3] > 0)
		{
			iFirstLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2];
			iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2] + aiLettersOnSide[3] - 1;
			iResult += DrawTextByRectangleLeftSide(iFirstLetter, iLastLetter);
		}

		return iResult;
	}

	int RectangleStampDrawer::DrawTextByRectangleTopSide(int iFirstChar, int iLastChar)
	{
		LONG lLetterWidth = m_sStampSize.cx / (iLastChar - iFirstChar + 1);
		COORD cCenterCoordinate;
		cCenterCoordinate.X = (SHORT)(m_cStampCoordinates.X + lLetterWidth / 2);
		cCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y - m_lFontHeight / 2);
		int iResult = 0;

		for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
		{
			iResult += (DrawSymbol(m_lpsText[iLetterCounter], cCenterCoordinate, lLetterWidth, 0) ? 1 : 0);
			cCenterCoordinate.X += (SHORT)lLetterWidth;
		}

		return iResult;
	}

	int RectangleStampDrawer::DrawTextByRectangleRightSide(int iFirstChar, int iLastChar)
	{
		LONG lLetterWidth = m_sStampSize.cy / (iLastChar - iFirstChar + 1);
		COORD cCenterCoordinate;
		cCenterCoordinate.X = (SHORT)(m_cStampCoordinates.X + m_sStampSize.cx + m_lFontHeight / 2);
		cCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y + lLetterWidth / 2);
		int iResult = 0;

		for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
		{
			iResult += (DrawSymbol(m_lpsText[iLetterCounter], cCenterCoordinate, lLetterWidth, 90) ? 1 : 0);
			cCenterCoordinate.Y += (SHORT)lLetterWidth;
		}

		return iResult;
	}

	int RectangleStampDrawer::DrawTextByRectangleBottomSide(int iFirstChar, int iLastChar)
	{
		LONG lLetterWidth = m_sStampSize.cx / (iLastChar - iFirstChar + 1);
		COORD cCenterCoordinate;
		cCenterCoordinate.X = (SHORT)(m_cStampCoordinates.X + m_sStampSize.cx - lLetterWidth / 2);
		cCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y + m_sStampSize.cy + m_lFontHeight / 2);
		int iResult = 0;

		for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
		{
			iResult += (DrawSymbol(m_lpsText[iLetterCounter], cCenterCoordinate, lLetterWidth, 180) ? 1 : 0);
			cCenterCoordinate.X -= (SHORT)lLetterWidth;
		}

		return iResult;
	}

	int RectangleStampDrawer::DrawTextByRectangleLeftSide(int iFirstChar, int iLastChar)
	{
		LONG lLetterWidth = m_sStampSize.cy / (iLastChar - iFirstChar + 1);
		COORD cCenterCoordinate;
		cCenterCoordinate.X = (SHORT)(m_cStampCoordinates.X - m_lFontHeight / 2);
		cCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y + m_sStampSize.cy - lLetterWidth / 2);
		int iResult = 0;

		for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
		{
			iResult += (DrawSymbol(m_lpsText[iLetterCounter], cCenterCoordinate, lLetterWidth, 270) ? 1 : 0);
			cCenterCoordinate.Y -= (SHORT)lLetterWidth;
		}

		return iResult;
	}

	BOOL RectangleStampDrawer::DrawSymbol(TCHAR cSymbol, COORD cCenterPoint, LONG lPlaceholderWidth, WORD wAngle)
	{
		HDC hWndDC = GetDC(m_hWnd);

		RECT rTextRect;
		rTextRect.top = cCenterPoint.Y - m_lFontHeight / 2;
		rTextRect.bottom = cCenterPoint.Y + m_lFontHeight / 2;
		rTextRect.left = cCenterPoint.X - lPlaceholderWidth / 2;
		rTextRect.right = cCenterPoint.X + lPlaceholderWidth / 2;

		HFONT hFont = FontProcessor::CreateFontByHeight(m_hWnd, m_lFontHeight);
		HGDIOBJ oldObject = SelectObject(hWndDC, hFont);

		XFORM xForm;
		int iPrevGraphicsMode = SetGraphicsMode(hWndDC, GM_ADVANCED);

		cCenterPoint.X = -cCenterPoint.X;
		cCenterPoint.Y = -cCenterPoint.Y;
		xForm = XformCreator::CreateMovementXform(cCenterPoint);
		SetWorldTransform(hWndDC, &xForm);

		xForm = XformCreator::CreateRotationXform(wAngle);
		ModifyWorldTransform(hWndDC, &xForm, MWT_RIGHTMULTIPLY);

		cCenterPoint.X = -cCenterPoint.X;
		cCenterPoint.Y = -cCenterPoint.Y;
		xForm = XformCreator::CreateMovementXform(cCenterPoint);
		ModifyWorldTransform(hWndDC, &xForm, MWT_RIGHTMULTIPLY);

		int iOldBkMode = SetBkMode(hWndDC, TRANSPARENT);
		LPTSTR lpsTextToDisplay = StringProcessor::CreateStringByChar(cSymbol);
		BOOL bResult = DrawText(hWndDC, lpsTextToDisplay, 1, &rTextRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		free(lpsTextToDisplay);
		SetBkMode(hWndDC, iOldBkMode);

		SelectObject(hWndDC, oldObject);
		DeleteObject(hFont);

		ModifyWorldTransform(hWndDC, NULL, MWT_IDENTITY);
		SetGraphicsMode(hWndDC, iPrevGraphicsMode);
		ReleaseDC(m_hWnd, hWndDC);

		return bResult;
	}

	void RectangleStampDrawer::GetSymbolsCountForRectangleSides(int iTextLength, int aiLettersCount[])
	{
		const BYTE cbRectangleSides = 4;
		int iGuaranteedSymbolsCount = iTextLength / cbRectangleSides, additionalSymbolsCount = iTextLength % cbRectangleSides;
		for (int i = 0; i < cbRectangleSides; i++)
		{
			aiLettersCount[i] = iGuaranteedSymbolsCount;
		}
		for (int i = 0; i < additionalSymbolsCount; i++)
		{
			aiLettersCount[i]++;
		}
	}

	RectangleStampDrawer::RectangleStampDrawer(HWND hWnd, COLORREF crImageBackgroundColor) : StampDrawer(hWnd, crImageBackgroundColor)
	{ }
}
