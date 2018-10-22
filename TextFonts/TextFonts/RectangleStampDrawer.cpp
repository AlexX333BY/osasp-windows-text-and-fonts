#include "RectangleStampDrawer.h"
#include "BitmapProcessor.h"
#include "FontProcessor.h"
#include "StringProcessor.h"
#include "WindowProcessor.h"
#include "XformCreator.h"
#include "DeviceContextProcessor.h"

namespace Stamp
{
	BOOL RectangleStampDrawer::Draw()
	{
		SIZE sWndSize = WindowProcessor::GetWindowSize(m_hWnd);
		RECT rWndRect;
		rWndRect.bottom = sWndSize.cy;
		rWndRect.left = 0;
		rWndRect.right = sWndSize.cx;
		rWndRect.top = 0;

		HDC hWndDC = GetDC(m_hWnd);
		HDC hBufferDC = CreateCompatibleDC(hWndDC);
		HBITMAP hBufferBitmap = CreateCompatibleBitmap(hWndDC, sWndSize.cx, sWndSize.cy);
		HGDIOBJ hOldDrawObject = SelectObject(hBufferDC, hBufferBitmap);

		DeviceContextProcessor::FillRectangle(hBufferDC, m_crImageBackgroundColor, rWndRect);
		BOOL bResult = TRUE;

		if (m_hBackgroundImage != NULL)
		{
			bResult &= DrawBackgroundImage(hBufferDC);
		}
		bResult &= (DrawTextByRectangle(hBufferDC) == GetTextLength());

		bResult &= BitBlt(hWndDC, 0, 0, sWndSize.cx, sWndSize.cy, hBufferDC, 0, 0, SRCCOPY);

		SelectObject(hBufferDC, hOldDrawObject);
		DeleteObject(hBufferBitmap);
		DeleteDC(hBufferDC);
		ReleaseDC(m_hWnd, hWndDC);

		return bResult;
	}

	BOOL RectangleStampDrawer::DrawBackgroundImage(HDC hDrawDC)
	{
		if (m_hBackgroundImage == NULL)
		{
			return FALSE;
		}
		
		SIZE sBackgroundSize = BitmapProcessor::GetBitmapSize(m_hBackgroundImage);
		HDC hBackgroundDC = CreateCompatibleDC(hDrawDC);
		HGDIOBJ hOldObject = SelectObject(hBackgroundDC, m_hBackgroundImage);

		BOOL bResult = StretchBlt(hDrawDC, m_cStampCoordinates.X, m_cStampCoordinates.Y, m_sStampSize.cx, m_sStampSize.cy, hBackgroundDC, 0, 0, sBackgroundSize.cx, sBackgroundSize.cy, SRCCOPY);

		SelectObject(hBackgroundDC, hOldObject);
		DeleteDC(hBackgroundDC);
		return bResult;
	}

	int RectangleStampDrawer::DrawTextByRectangle(HDC hDrawDC)
	{
		int aiLettersOnSide[4];
		GetSymbolsCountForRectangleSides(lstrlen(m_lpsText), aiLettersOnSide);
		int iFirstLetter, iLastLetter;
		int iResult = 0;

		if (aiLettersOnSide[0] > 0)
		{
			iFirstLetter = 0;
			iLastLetter = aiLettersOnSide[0] - 1;
			iResult += DrawTextByRectangleTopSide(hDrawDC, iFirstLetter, iLastLetter);
		}

		if (aiLettersOnSide[1] > 0)
		{
			iFirstLetter = aiLettersOnSide[0];
			iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] - 1;
			iResult += DrawTextByRectangleRightSide(hDrawDC, iFirstLetter, iLastLetter);
		}

		if (aiLettersOnSide[2] > 0)
		{
			iFirstLetter = aiLettersOnSide[0] + aiLettersOnSide[1];
			iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2] - 1;
			iResult += DrawTextByRectangleBottomSide(hDrawDC, iFirstLetter, iLastLetter);
		}

		if (aiLettersOnSide[3] > 0)
		{
			iFirstLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2];
			iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2] + aiLettersOnSide[3] - 1;
			iResult += DrawTextByRectangleLeftSide(hDrawDC, iFirstLetter, iLastLetter);
		}

		return iResult;
	}

	int RectangleStampDrawer::DrawTextByRectangleTopSide(HDC hDrawDC, int iFirstChar, int iLastChar)
	{
		LONG lLetterWidth = m_sStampSize.cx / (iLastChar - iFirstChar + 1);
		COORD cCenterCoordinate;
		cCenterCoordinate.X = (SHORT)(m_cStampCoordinates.X + lLetterWidth / 2);
		cCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y - m_lFontHeight / 2);
		int iResult = 0;

		for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
		{
			iResult += (DrawSymbol(hDrawDC, m_lpsText[iLetterCounter], cCenterCoordinate, lLetterWidth, 0) ? 1 : 0);
			cCenterCoordinate.X += (SHORT)lLetterWidth;
		}

		return iResult;
	}

	int RectangleStampDrawer::DrawTextByRectangleRightSide(HDC hDrawDC, int iFirstChar, int iLastChar)
	{
		LONG lLetterWidth = m_sStampSize.cy / (iLastChar - iFirstChar + 1);
		COORD cCenterCoordinate;
		cCenterCoordinate.X = (SHORT)(m_cStampCoordinates.X + m_sStampSize.cx + m_lFontHeight / 2);
		cCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y + lLetterWidth / 2);
		int iResult = 0;

		for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
		{
			iResult += (DrawSymbol(hDrawDC, m_lpsText[iLetterCounter], cCenterCoordinate, lLetterWidth, 90) ? 1 : 0);
			cCenterCoordinate.Y += (SHORT)lLetterWidth;
		}

		return iResult;
	}

	int RectangleStampDrawer::DrawTextByRectangleBottomSide(HDC hDrawDC, int iFirstChar, int iLastChar)
	{
		LONG lLetterWidth = m_sStampSize.cx / (iLastChar - iFirstChar + 1);
		COORD cCenterCoordinate;
		cCenterCoordinate.X = (SHORT)(m_cStampCoordinates.X + m_sStampSize.cx - lLetterWidth / 2);
		cCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y + m_sStampSize.cy + m_lFontHeight / 2);
		int iResult = 0;

		for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
		{
			iResult += (DrawSymbol(hDrawDC, m_lpsText[iLetterCounter], cCenterCoordinate, lLetterWidth, 180) ? 1 : 0);
			cCenterCoordinate.X -= (SHORT)lLetterWidth;
		}

		return iResult;
	}

	int RectangleStampDrawer::DrawTextByRectangleLeftSide(HDC hDrawDC, int iFirstChar, int iLastChar)
	{
		LONG lLetterWidth = m_sStampSize.cy / (iLastChar - iFirstChar + 1);
		COORD cCenterCoordinate;
		cCenterCoordinate.X = (SHORT)(m_cStampCoordinates.X - m_lFontHeight / 2);
		cCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y + m_sStampSize.cy - lLetterWidth / 2);
		int iResult = 0;

		for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
		{
			iResult += (DrawSymbol(hDrawDC, m_lpsText[iLetterCounter], cCenterCoordinate, lLetterWidth, 270) ? 1 : 0);
			cCenterCoordinate.Y -= (SHORT)lLetterWidth;
		}

		return iResult;
	}

	BOOL RectangleStampDrawer::DrawSymbol(HDC hDrawDC, TCHAR cSymbol, COORD cCenterPoint, LONG lPlaceholderWidth, WORD wAngle)
	{
		RECT rTextRect;
		rTextRect.top = cCenterPoint.Y - m_lFontHeight / 2;
		rTextRect.bottom = cCenterPoint.Y + m_lFontHeight / 2;
		rTextRect.left = cCenterPoint.X - lPlaceholderWidth / 2;
		rTextRect.right = cCenterPoint.X + lPlaceholderWidth / 2;

		HFONT hFont = FontProcessor::CreateFontByHeight(m_hWnd, m_lFontHeight);
		HGDIOBJ oldObject = SelectObject(hDrawDC, hFont);

		XFORM xForm;
		int iPrevGraphicsMode = SetGraphicsMode(hDrawDC, GM_ADVANCED);

		cCenterPoint.X = -cCenterPoint.X;
		cCenterPoint.Y = -cCenterPoint.Y;
		xForm = XformCreator::CreateMovementXform(cCenterPoint);
		SetWorldTransform(hDrawDC, &xForm);

		xForm = XformCreator::CreateRotationXform(wAngle);
		ModifyWorldTransform(hDrawDC, &xForm, MWT_RIGHTMULTIPLY);

		cCenterPoint.X = -cCenterPoint.X;
		cCenterPoint.Y = -cCenterPoint.Y;
		xForm = XformCreator::CreateMovementXform(cCenterPoint);
		ModifyWorldTransform(hDrawDC, &xForm, MWT_RIGHTMULTIPLY);

		int iOldBkMode = SetBkMode(hDrawDC, TRANSPARENT);
		LPTSTR lpsTextToDisplay = StringProcessor::CreateStringByChar(cSymbol);
		BOOL bResult = DrawText(hDrawDC, lpsTextToDisplay, 1, &rTextRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		free(lpsTextToDisplay);
		SetBkMode(hDrawDC, iOldBkMode);

		SelectObject(hDrawDC, oldObject);
		DeleteObject(hFont);

		ModifyWorldTransform(hDrawDC, NULL, MWT_IDENTITY);
		SetGraphicsMode(hDrawDC, iPrevGraphicsMode);

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
