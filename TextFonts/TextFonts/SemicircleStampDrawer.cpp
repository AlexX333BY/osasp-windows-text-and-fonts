#include "SemicircleStampDrawer.h"
#include "FontProcessor.h"
#include "XformCreator.h"
#include "StringProcessor.h"
#include "WindowProcessor.h"
#include "DeviceContextProcessor.h"
#include "BitmapProcessor.h"

#define _USE_MATH_DEFINES 

#include <cmath>

namespace Stamp
{
	BOOL SemicircleStampDrawer::Draw()
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
			bResult = DrawBackgroundImage(hBufferDC);
		}

		int iBottomSymbolsCount = GetBottomSymbolsCount();
		int iArcSymbolsCount = lstrlen(m_lpsText) - iBottomSymbolsCount;
		
		if (iBottomSymbolsCount != 0)
		{
			bResult &= (DrawBottomSymbols(hBufferDC, iArcSymbolsCount) == iBottomSymbolsCount);
		}
		if (iArcSymbolsCount != 0)
		{
			bResult &= (DrawArcSymbols(hBufferDC, iArcSymbolsCount) == iArcSymbolsCount);
		}

		bResult &= BitBlt(hWndDC, 0, 0, sWndSize.cx, sWndSize.cy, hBufferDC, 0, 0, SRCCOPY);

		SelectObject(hBufferDC, hOldDrawObject);
		DeleteObject(hBufferBitmap);
		DeleteDC(hBufferDC);
		ReleaseDC(m_hWnd, hWndDC);

		return bResult;
	}

	BOOL SemicircleStampDrawer::DrawBackgroundImage(HDC hDrawDC)
	{
		if (m_hBackgroundImage == NULL)
		{
			return FALSE;
		}

		HBITMAP hImageCopy = (HBITMAP)CopyImage(m_hBackgroundImage, IMAGE_BITMAP, 0, 0, 0);

		HDC hImageDC = CreateCompatibleDC(hDrawDC);
		SIZE sDcSize = BitmapProcessor::GetBitmapSize(m_hBackgroundImage);
		HGDIOBJ hOldImageObject = SelectObject(hImageDC, hImageCopy);

		HDC hMaskDC = CreateCompatibleDC(hDrawDC);
		HBITMAP hMaskBitmap = CreateCompatibleBitmap(hDrawDC, sDcSize.cx, sDcSize.cy);
		HGDIOBJ hOldMaskObject = SelectObject(hMaskDC, hMaskBitmap);

		RECT rDcRect;
		rDcRect.bottom = sDcSize.cy;
		rDcRect.left = 0;
		rDcRect.right = sDcSize.cx;
		rDcRect.top = 0;
		RECT rEllipseRect = rDcRect;
		rEllipseRect.bottom = sDcSize.cy * 2;

		DeviceContextProcessor::FillRectangle(hMaskDC, RGB(0x00, 0x00, 0x00), rDcRect);
		DeviceContextProcessor::FillEllipse(hMaskDC, RGB(0xFF, 0xFF, 0xFF), rEllipseRect);

		BOOL bResult = BitBlt(hImageDC, 0, 0, sDcSize.cx, sDcSize.cy, hMaskDC, 0, 0, SRCAND);

		DeviceContextProcessor::FillRectangle(hMaskDC, m_crImageBackgroundColor, rDcRect);
		DeviceContextProcessor::FillEllipse(hMaskDC, RGB(0x00, 0x00, 0x00), rEllipseRect);

		bResult &= BitBlt(hImageDC, 0, 0, sDcSize.cx, sDcSize.cy, hMaskDC, 0, 0, SRCPAINT);

		bResult &= StretchBlt(hDrawDC, m_cStampCoordinates.X, m_cStampCoordinates.Y, m_sStampSize.cx, m_sStampSize.cy, hImageDC, 0, 0, sDcSize.cx, sDcSize.cy, SRCCOPY);

		SelectObject(hImageDC, hOldImageObject);
		SelectObject(hMaskDC, hOldMaskObject);
		DeleteObject(hMaskBitmap);
		DeleteObject(hImageCopy);
		DeleteDC(hMaskDC);
		DeleteDC(hImageDC);

		return bResult;
	}

	int SemicircleStampDrawer::DrawBottomSymbols(HDC hDrawDC, int iStartSymbol)
	{
		int iTextLength = lstrlen(m_lpsText);
		DOUBLE lPlaceholderWidth = (DOUBLE)m_sStampSize.cx / (iTextLength - iStartSymbol);

		DOUBLE curXCoordinate = m_cStampCoordinates.X + m_sStampSize.cx - lPlaceholderWidth / 2;
		COORD cSymbolCenterCoordinate;
		cSymbolCenterCoordinate.X = (SHORT)curXCoordinate;
		cSymbolCenterCoordinate.Y = (SHORT)(m_cStampCoordinates.Y + m_sStampSize.cy + m_lFontHeight / 2);

		int iResult = 0;
		for (int iSymbolCounter = iStartSymbol; iSymbolCounter < iTextLength; iSymbolCounter++)
		{
			iResult += (DrawSymbol(hDrawDC, m_lpsText[iSymbolCounter], cSymbolCenterCoordinate, (LONG)lPlaceholderWidth, 180) ? 1 : 0);
			curXCoordinate -= lPlaceholderWidth;
			cSymbolCenterCoordinate.X = (SHORT)curXCoordinate;
		}

		return iResult;
	}

	int SemicircleStampDrawer::DrawArcSymbols(HDC hDrawDC, int iArcSymbolsCount)
	{
		DOUBLE dAngleStep = 180.0 / (iArcSymbolsCount + 1);
		DOUBLE dCurAngle = 180 - dAngleStep;
		LONG lPlaceholderWidth = (LONG)(M_PI * (m_sStampSize.cx + m_sStampSize.cy) / 4 / iArcSymbolsCount);

		int iResult = 0;
		for (int iSymbolCounter = 0; iSymbolCounter < iArcSymbolsCount; iSymbolCounter++)
		{
			iResult += (DrawSymbol(hDrawDC, m_lpsText[iSymbolCounter], GetArcSymbolCenterCoordinates(dCurAngle), lPlaceholderWidth, 90 - dCurAngle) ? 1 : 0);
			dCurAngle -= dAngleStep;
		}

		return iResult;
	}

	int SemicircleStampDrawer::GetBottomSymbolsCount()
	{
		return (int)(lstrlen(m_lpsText) * m_sStampSize.cx / (M_PI * (m_sStampSize.cx + m_sStampSize.cy) / 4 + m_sStampSize.cx));
	}

	COORD SemicircleStampDrawer::GetArcSymbolCenterCoordinates(DOUBLE dAngle)
	{
		LONG lEllipseA = m_sStampSize.cx / 2, lEllipseB = m_sStampSize.cy;
		DOUBLE dRadAngle = M_PI * dAngle / 180.0;
		DOUBLE dAngleSin = sin(dRadAngle), dAngleCos = cos(dRadAngle);
		DOUBLE dRadius = sqrt(pow(lEllipseA, 2) * pow(lEllipseB, 2) / (pow(dAngleSin, 2) * pow(lEllipseA, 2) + pow(dAngleCos, 2) * pow(lEllipseB, 2)));

		COORD cSymbolCenterCoord;
		cSymbolCenterCoord.X = (SHORT)(m_cStampCoordinates.X + m_sStampSize.cx / 2 + (dRadius + m_lFontHeight / 2) * dAngleCos);
		cSymbolCenterCoord.Y = (SHORT)(m_cStampCoordinates.Y + m_sStampSize.cy - (dRadius + m_lFontHeight / 2) * dAngleSin);
		return cSymbolCenterCoord;
	}

	BOOL SemicircleStampDrawer::DrawSymbol(HDC hDrawDC, TCHAR cSymbol, COORD cCenterPoint, LONG lPlaceholderWidth, DOUBLE dAngle)
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

		xForm = XformCreator::CreateRotationXform(dAngle);
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

	SemicircleStampDrawer::SemicircleStampDrawer(HWND hWnd, COLORREF crImageBackgroundColor) : StampDrawer::StampDrawer(hWnd, crImageBackgroundColor)
	{ }
}
