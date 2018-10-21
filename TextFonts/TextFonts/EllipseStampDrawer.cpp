#include "EllipseStampDrawer.h"
#include "BitmapProcessor.h"
#include "DeviceContextProcessor.h"
#include "FontProcessor.h"
#include "StringProcessor.h"
#include "WindowProcessor.h"
#include "XformCreator.h"

#define _USE_MATH_DEFINES 

#include <cmath>

namespace Stamp
{
	BOOL EllipseStampDrawer::Draw()
	{
		WindowProcessor::FillWindowWithColor(m_hWnd, m_crImageBackgroundColor);
		BOOL bResult = DrawBackgroundImage();

		int iTextLength = GetTextLength();
		if (iTextLength == 0)
		{
			return bResult;
		}

		LONG lSymbolWidth = (LONG)(M_PI * (m_sStampSize.cx + m_sStampSize.cy) / 2 / iTextLength);
		DOUBLE dAngleStep = 360.0 / iTextLength;
		DOUBLE dCurAngle = 90, dCurRotateAngle = 0;
		COORD cSymbolCoord;
		int iTotalDrawedSymbols = 0;
		for (int iSymbol = 0; iSymbol < iTextLength; iSymbol++)
		{
			cSymbolCoord = GetSymbolCenterCoordinates(dCurAngle);
			iTotalDrawedSymbols += (DrawSymbol(m_lpsText[iSymbol], cSymbolCoord, lSymbolWidth, dCurRotateAngle) ? 1 : 0);

			dCurAngle -= dAngleStep;
			if (dCurAngle < 0)
			{
				dCurAngle += 360;
			}

			dCurRotateAngle += dAngleStep;
			if (dCurRotateAngle < 0)
			{
				dCurRotateAngle += 360;
			}
		}

		return bResult && (iTotalDrawedSymbols == iTextLength);
	}

	BOOL EllipseStampDrawer::DrawBackgroundImage()
	{
		if (m_hBackgroundImage == NULL)
		{
			return FALSE;
		}

		HDC hWndDC = GetDC(m_hWnd);

		HDC hImageDC = CreateCompatibleDC(hWndDC);
		SIZE sDcSize = BitmapProcessor::GetBitmapSize(m_hBackgroundImage);
		HGDIOBJ hOldImageObject = SelectObject(hImageDC, m_hBackgroundImage);

		HDC hMaskDC = CreateCompatibleDC(hWndDC);
		HBITMAP hMaskBitmap = CreateCompatibleBitmap(hMaskDC, sDcSize.cx, sDcSize.cy);
		HGDIOBJ hOldMaskObject = SelectObject(hMaskDC, hMaskBitmap);

		RECT rDcRect;
		rDcRect.bottom = sDcSize.cy;
		rDcRect.left = 0;
		rDcRect.right = sDcSize.cx;
		rDcRect.top = 0;

		DeviceContextProcessor::FillRectangle(hMaskDC, RGB(0x00, 0x00, 0x00), rDcRect);
		DeviceContextProcessor::FillEllipse(hMaskDC, RGB(0xFF, 0xFF, 0xFF), rDcRect);

		BOOL bResult = BitBlt(hImageDC, 0, 0, sDcSize.cx, sDcSize.cy, hMaskDC, 0, 0, SRCAND);

		DeviceContextProcessor::FillRectangle(hMaskDC, m_crImageBackgroundColor, rDcRect);
		DeviceContextProcessor::FillEllipse(hMaskDC, RGB(0x00, 0x00, 0x00), rDcRect);

		bResult &= BitBlt(hImageDC, 0, 0, sDcSize.cx, sDcSize.cy, hMaskDC, 0, 0, SRCPAINT);

		bResult &= StretchBlt(hWndDC, m_cStampCoordinates.X, m_cStampCoordinates.Y, m_sStampSize.cx, m_sStampSize.cy, hImageDC, 0, 0, sDcSize.cx, sDcSize.cy, SRCCOPY);
		
		SelectObject(hImageDC, hOldImageObject);
		SelectObject(hMaskDC, hOldMaskObject);
		DeleteObject(hMaskBitmap);
		DeleteDC(hMaskDC);
		DeleteDC(hImageDC);
		ReleaseDC(m_hWnd, hWndDC);
		return bResult;
	}

	COORD EllipseStampDrawer::GetSymbolCenterCoordinates(DOUBLE dAngle)
	{
		LONG lEllipseA = m_sStampSize.cx / 2, lEllipseB = m_sStampSize.cy / 2;
		DOUBLE dRadAngle = M_PI * dAngle / 180.0;
		DOUBLE dAngleSin = sin(dRadAngle), dAngleCos = cos(dRadAngle);
		DOUBLE dRadius = sqrt(pow(lEllipseA, 2) * pow(lEllipseB, 2) / (pow(dAngleSin, 2) * pow(lEllipseA, 2) + pow(dAngleCos, 2) * pow(lEllipseB, 2)));
		
		COORD cSymbolCenterCoord;
		cSymbolCenterCoord.X = (SHORT)(m_cStampCoordinates.X + m_sStampSize.cx / 2 + (dRadius + m_lFontHeight / 2) * dAngleCos);
		cSymbolCenterCoord.Y = (SHORT)(m_cStampCoordinates.Y + m_sStampSize.cy / 2 - (dRadius + m_lFontHeight / 2) * dAngleSin);
		return cSymbolCenterCoord;
	}

	BOOL EllipseStampDrawer::DrawSymbol(TCHAR cSymbol, COORD cCenterPoint, LONG lPlaceholderWidth, DOUBLE dAngle)
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

		xForm = XformCreator::CreateRotationXform(dAngle);
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

	EllipseStampDrawer::EllipseStampDrawer(HWND hWnd, COLORREF crImageBackgroundColor) : StampDrawer(hWnd, crImageBackgroundColor)
	{ }
}
