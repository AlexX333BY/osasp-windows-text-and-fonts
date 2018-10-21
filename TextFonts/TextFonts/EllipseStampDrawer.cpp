#include "EllipseStampDrawer.h"
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
		int iTextLength = GetTextLength();

		if (iTextLength == 0)
		{
			return TRUE;
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

		return iTotalDrawedSymbols == iTextLength;
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
