#include <windows.h>
#include <strsafe.h>

#define _USE_MATH_DEFINES 

#include <cmath>

#define WM_DRAW_STAMP WM_USER

namespace defaults
{
	const BYTE cbLeftIndentPercents = 10;
	const BYTE cbRightIndentPercents = 10;
	const BYTE cbUpIndentPercents = 10;
	const BYTE cbDownIndentPercents = 10;
}

DWORD GetBackgroundColor()
{
	return GetSysColor(COLOR_WINDOW);
}

BOOL IsBetween(INT64 iValue, INT64 iLeftBound, INT64 iRightBound)
{
	return (iValue >= iLeftBound) && (iValue <= iRightBound);
}

SIZE GetClientWindowSize(HWND hWnd)
{
	RECT rWndRect;
	GetClientRect(hWnd, &rWndRect);
	SIZE sWndSize;
	sWndSize.cx = rWndRect.right - rWndRect.left;
	sWndSize.cy = rWndRect.bottom - rWndRect.top;
	return sWndSize;
}

int FillWindowWithColor(HWND hWnd, COLORREF crColor)
{
	RECT rWndRect;
	GetClientRect(hWnd, &rWndRect);
	HDC hWndDC = GetDC(hWnd);
	HBRUSH hBrush = CreateSolidBrush(crColor);
	int iResult = FillRect(hWndDC, &rWndRect, hBrush);
	DeleteObject(hBrush);
	ReleaseDC(hWnd, hWndDC);
	return iResult;
}

BOOL UpdateStampPosition(HWND hWnd, COORD &rcStampCoords, SIZE &rsStampSize,
	BYTE cbLeftIndentPercents = defaults::cbLeftIndentPercents, 
	BYTE cbRightIndentPercents = defaults::cbRightIndentPercents,
	BYTE cbUpIndentPercents = defaults::cbUpIndentPercents, 
	BYTE cbDownIndentPercents = defaults::cbDownIndentPercents)
{
	if (!IsBetween(cbLeftIndentPercents, 0, 100) 
		|| !IsBetween(cbRightIndentPercents, 0, 100)
		|| !IsBetween(cbUpIndentPercents, 0, 100) 
		|| !IsBetween(cbDownIndentPercents, 0, 100)
		|| (cbLeftIndentPercents + cbRightIndentPercents >= 100) 
		|| (cbDownIndentPercents + cbUpIndentPercents >= 100))
	{
		return FALSE;
	}

	SIZE sWndSize = GetClientWindowSize(hWnd);
	rcStampCoords.X = (SHORT)(sWndSize.cx * cbLeftIndentPercents / 100);
	rcStampCoords.Y = (SHORT)(sWndSize.cy * cbUpIndentPercents / 100);
	rsStampSize.cx = (SHORT)(sWndSize.cx * (100 - cbLeftIndentPercents 
		- cbRightIndentPercents) / 100);
	rsStampSize.cy = (SHORT)(sWndSize.cy * (100 - cbDownIndentPercents 
		- cbUpIndentPercents) / 100);
	return TRUE;
}

LPTSTR GetEmptyString()
{
	LPTSTR lpsEmptyText = (LPTSTR)(calloc(1, sizeof(TCHAR)));
	lpsEmptyText[0] = '\0';
	return lpsEmptyText;

}

LPTSTR DeleteLastChar(LPTSTR lpsText)
{
	int iTextLength = lstrlen(lpsText);
	if (iTextLength == 0)
	{
		return GetEmptyString();
	}
	else
	{
		LPTSTR lpsNewText = (LPTSTR)(calloc(iTextLength, sizeof(TCHAR)));
		if (StringCchCopyN(lpsNewText, iTextLength, lpsText, iTextLength - 1) == S_OK)
		{
			return lpsNewText;
		}
		else
		{
			free(lpsNewText);
			return NULL;
		}
	}
}

LPTSTR ConcatStringAndChar(LPTSTR lpsText, TCHAR cChar)
{
	int iNewLength = lstrlen(lpsText) + 2;
	LPTSTR lpsNewText = (LPTSTR)(calloc(iNewLength, sizeof(TCHAR)));
	if (StringCchCopy(lpsNewText, iNewLength, lpsText) == S_OK)
	{
		lpsNewText[iNewLength - 2] = cChar;
		lpsNewText[iNewLength - 1] = '\0';
		return lpsNewText;
	}
	else
	{
		free(lpsNewText);
		return NULL;
	}
}

LONG GetFontHeight(HWND hWnd)
{
	HDC hWndDC = GetDC(hWnd);
	TEXTMETRIC tmTextMetric;
	GetTextMetrics(hWndDC, &tmTextMetric);
	ReleaseDC(hWnd, hWndDC);
	return tmTextMetric.tmHeight;
}

XFORM GetRotationXform(WORD wDegreeAngle)
{
	XFORM xForm;
	FLOAT fRadAngle = (FLOAT)(M_PI * wDegreeAngle / 180);
	FLOAT fAngleSin = sin(fRadAngle);
	FLOAT fAngleCos = cos(fRadAngle);
	xForm.eM11 = fAngleCos;
	xForm.eM12 = fAngleSin;
	xForm.eM21 = -fAngleSin;
	xForm.eM22 = fAngleCos;
	xForm.eDx = 0;
	xForm.eDy = 0;
	return xForm;
}

XFORM GetMovementXform(COORD cCoordinates)
{
	XFORM xForm;
	xForm.eM11 = 1;
	xForm.eM12 = 0;
	xForm.eM21 = 0;
	xForm.eM22 = 1;
	xForm.eDx = cCoordinates.X;
	xForm.eDy = cCoordinates.Y;
	return xForm;
}

BOOL PlaceTextByLine(HWND hWnd, COORD cCenterPoint, SIZE sTextRectSize, WORD wAngle, LPTSTR lpsText, DWORD dwFirstSymbol, DWORD dwLastSymbol)
{
	int iFullTextLength = lstrlen(lpsText);
	if (dwFirstSymbol >= iFullTextLength)
	{
		return FALSE;
	}
	int iDisplayedTextLength = min(dwLastSymbol, iFullTextLength - 1) - dwFirstSymbol + 1;
	if (iDisplayedTextLength < 1)
	{
		return FALSE;
	}
	LPSTR lpsDisplayedText = (LPSTR)calloc(iDisplayedTextLength + 1, sizeof(TCHAR));
	for (int i = 0; i < iDisplayedTextLength; i++)
	{
		lpsDisplayedText[i] = lpsText[i + dwFirstSymbol];
	}

	HDC hWndDC = GetDC(hWnd);
	RECT rTextRect;
	rTextRect.top = cCenterPoint.Y - sTextRectSize.cy / 2;
	rTextRect.bottom = cCenterPoint.Y + sTextRectSize.cy / 2;
	rTextRect.left = cCenterPoint.X - sTextRectSize.cx / 2;
	rTextRect.right = cCenterPoint.X + sTextRectSize.cx / 2;

	XFORM xForm;
	int iPrevGraphicsMode = SetGraphicsMode(hWndDC, GM_ADVANCED);

	cCenterPoint.X = -cCenterPoint.X;
	cCenterPoint.Y = -cCenterPoint.Y;
	xForm = GetMovementXform(cCenterPoint);
	SetWorldTransform(hWndDC, &xForm);

	xForm = GetRotationXform(wAngle);
	ModifyWorldTransform(hWndDC, &xForm, MWT_RIGHTMULTIPLY);

	cCenterPoint.X = -cCenterPoint.X;
	cCenterPoint.Y = -cCenterPoint.Y;
	xForm = GetMovementXform(cCenterPoint);
	ModifyWorldTransform(hWndDC, &xForm, MWT_RIGHTMULTIPLY);

	BOOL bResult = DrawText(hWndDC, lpsDisplayedText, iDisplayedTextLength, &rTextRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_WORDBREAK);

	ModifyWorldTransform(hWndDC, NULL, MWT_IDENTITY);
	SetGraphicsMode(hWndDC, iPrevGraphicsMode);
	ReleaseDC(hWnd, hWndDC);

	return bResult;
}

void GetLettersCountForRectangleSides(int iTextLength, int aiLettersCount[])
{
	const BYTE cbRectangleSides = 4;
	int iGuaranteedLettersCount = iTextLength / cbRectangleSides, additionalLetterCount = iTextLength % cbRectangleSides;
	for (int i = 0; i < cbRectangleSides; i++)
	{
		aiLettersCount[i] = iGuaranteedLettersCount;
	}
	for (int i = 0; i < additionalLetterCount; i++)
	{
		aiLettersCount[i]++;
	}
}

BOOL PlaceTextByRectangleUpperSide(HWND hWnd, COORD cRectangleCoordinate,
	SIZE sRectangleSize, LPTSTR lpsText, DWORD dwFirstSymbol, DWORD dwLastSymbol)
{
	COORD cCenterPoint;
	cCenterPoint.X = (SHORT)(cRectangleCoordinate.X + sRectangleSize.cx / 2);
	cCenterPoint.Y = cRectangleCoordinate.Y;
	SIZE sTextRectangleSize;
	sTextRectangleSize.cx = sRectangleSize.cx;
	sTextRectangleSize.cy = GetFontHeight(hWnd);
	return PlaceTextByLine(hWnd, cCenterPoint, sTextRectangleSize, 0, lpsText, dwFirstSymbol, dwLastSymbol);
}

BOOL PlaceTextByRectangleLeftSide(HWND hWnd, COORD cRectangleCoordinate,
	SIZE sRectangleSize, LPTSTR lpsText, DWORD dwFirstSymbol, DWORD dwLastSymbol)
{
	COORD cCenterPoint;
	cCenterPoint.X = cRectangleCoordinate.X;
	cCenterPoint.Y = (SHORT)(cRectangleCoordinate.Y + sRectangleSize.cy / 2);
	SIZE sTextRectangleSize;
	sTextRectangleSize.cx = sRectangleSize.cy;
	sTextRectangleSize.cy = GetFontHeight(hWnd);
	return PlaceTextByLine(hWnd, cCenterPoint, sTextRectangleSize, 270, lpsText, dwFirstSymbol, dwLastSymbol);
}

BOOL PlaceTextByRectangleLowerSide(HWND hWnd, COORD cRectangleCoordinate,
	SIZE sRectangleSize, LPTSTR lpsText, DWORD dwFirstSymbol, DWORD dwLastSymbol)
{
	COORD cCenterPoint;
	cCenterPoint.X = (SHORT)(cRectangleCoordinate.X + sRectangleSize.cx / 2);
	cCenterPoint.Y = (SHORT)(cRectangleCoordinate.Y + sRectangleSize.cy);
	SIZE sTextRectangleSize;
	sTextRectangleSize.cx = sRectangleSize.cx;
	sTextRectangleSize.cy = GetFontHeight(hWnd);
	return PlaceTextByLine(hWnd, cCenterPoint, sTextRectangleSize, 180, lpsText, dwFirstSymbol, dwLastSymbol);
}

BOOL PlaceTextByRectangleRightSide(HWND hWnd, COORD cRectangleCoordinate,
	SIZE sRectangleSize, LPTSTR lpsText, DWORD dwFirstSymbol, DWORD dwLastSymbol)
{
	COORD cCenterPoint;
	cCenterPoint.X = (SHORT)(cRectangleCoordinate.X + sRectangleSize.cx);
	cCenterPoint.Y = (SHORT)(cRectangleCoordinate.Y + sRectangleSize.cy / 2);
	SIZE sTextRectangleSize;
	sTextRectangleSize.cx = sRectangleSize.cy;
	sTextRectangleSize.cy = GetFontHeight(hWnd);
	return PlaceTextByLine(hWnd, cCenterPoint, sTextRectangleSize, 90, lpsText, dwFirstSymbol, dwLastSymbol);
}

BOOL PlaceTextByRectangle(HWND hWnd, COORD cRectangleCoordinates, SIZE sRectangleSize, LPTSTR lpsText)
{
	const BYTE cbRectangleSides = 4;
	int aiLettersOnSide[4];
	GetLettersCountForRectangleSides(lstrlen(lpsText), aiLettersOnSide);
	DWORD dwFirstLetter, dwLastLetter;
	BOOL bResult = TRUE;
	if (aiLettersOnSide[0] > 0)
	{
		dwFirstLetter = 0;
		dwLastLetter = aiLettersOnSide[0] - 1;
		bResult &= PlaceTextByRectangleUpperSide(hWnd, cRectangleCoordinates, sRectangleSize, lpsText, dwFirstLetter, dwLastLetter);
	}
	if (aiLettersOnSide[1] > 0)
	{
		dwFirstLetter = aiLettersOnSide[0];
		dwLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] - 1;
		bResult &= PlaceTextByRectangleRightSide(hWnd, cRectangleCoordinates, sRectangleSize, lpsText, dwFirstLetter, dwLastLetter);
	}
	if (aiLettersOnSide[2] > 0)
	{
		dwFirstLetter = aiLettersOnSide[0] + aiLettersOnSide[1];
		dwLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2] - 1;
		bResult &= PlaceTextByRectangleLowerSide(hWnd, cRectangleCoordinates, sRectangleSize, lpsText, dwFirstLetter, dwLastLetter);
	}
	if (aiLettersOnSide[3] > 0)
	{
		dwFirstLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2];
		dwLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2] + aiLettersOnSide[3] - 1;
		bResult &= PlaceTextByRectangleLeftSide(hWnd, cRectangleCoordinates, sRectangleSize, lpsText, dwFirstLetter, dwLastLetter);
	}
	return bResult;
}

void PostDrawStampMessage(HWND hWnd)
{
	PostMessage(hWnd, WM_DRAW_STAMP, NULL, NULL);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static COORD cStampCoordinates = { 0 };
	static SIZE sStampSize = { 0 };
	static LPTSTR lpsText = GetEmptyString();

	switch (message)
	{
	case WM_SIZE:
		UpdateStampPosition(hWnd, cStampCoordinates, sStampSize);
		PostDrawStampMessage(hWnd);
		DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_CHAR:
		LPTSTR lpsNewText;
		switch (wParam)
		{
		case VK_BACK:
			if ((lpsNewText = DeleteLastChar(lpsText)) != NULL)
			{
				free(lpsText);
				lpsText = lpsNewText;
				PostDrawStampMessage(hWnd);
			}
		case VK_RETURN:
		case VK_ESCAPE:
			break;
		default:
			if ((lpsNewText = ConcatStringAndChar(lpsText, (TCHAR)wParam)) != NULL)
			{
				free(lpsText);
				lpsText = lpsNewText;
				PostDrawStampMessage(hWnd);
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_DRAW_STAMP:
		FillWindowWithColor(hWnd, GetBackgroundColor());
		PlaceTextByRectangle(hWnd, cStampCoordinates, sStampSize, lpsText);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPTSTR lpCmdLine, int nCmdShow)
{
	PCSTR spWndClassName = "LaboratoryWork2Class";

	WNDCLASSEX wndClassEx;
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.style = CS_DBLCLKS;
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hInstance = hInstance;
	wndClassEx.hIcon = NULL;
	wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClassEx.hbrBackground = CreateSolidBrush(GetBackgroundColor());
	wndClassEx.lpszMenuName = NULL;
	wndClassEx.lpszClassName = spWndClassName;
	wndClassEx.hIconSm = NULL;
	RegisterClassEx(&wndClassEx);

	HWND hWnd = CreateWindow(spWndClassName, "Task #2", WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, 
		hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
