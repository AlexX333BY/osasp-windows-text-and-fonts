#include "DeviceContextProcessor.h"

namespace Stamp
{
	int DeviceContextProcessor::FillRectangle(HDC hDC, COLORREF crColor, RECT rRectToFill)
	{
		HBRUSH hBrush = CreateSolidBrush(crColor);
		int iResult = FillRect(hDC, &rRectToFill, hBrush);
		DeleteObject(hBrush);
		return iResult;
	}

	BOOL DeviceContextProcessor::FillEllipse(HDC hDC, COLORREF crFillColor, RECT rEllipseRect)
	{
		HBRUSH hBrush = CreateSolidBrush(crFillColor);
		HPEN hPen = CreatePen(PS_NULL, 0, crFillColor);
		HGDIOBJ hOldBrush = SelectObject(hDC, hBrush);
		HGDIOBJ hOldPen = SelectObject(hDC, hPen);

		BOOL bResult = Ellipse(hDC, rEllipseRect.left, rEllipseRect.top, rEllipseRect.right, rEllipseRect.bottom);

		SelectObject(hDC, hOldBrush);
		SelectObject(hDC, hOldPen);
		DeleteObject(hBrush);
		DeleteObject(hPen);

		return bResult;
	}
}
