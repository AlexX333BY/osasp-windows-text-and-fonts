#include <windows.h>
#include <strsafe.h>
#include <gdiplus.h>

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

LPTSTR CreateStringByChar(TCHAR cChar)
{
	LPTSTR lpsCharString = (LPTSTR)calloc(2, sizeof(TCHAR));
	lpsCharString[0] = cChar;
	lpsCharString[1] = '\0';
	return lpsCharString;
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

HFONT CreateFontByHeight(HWND hWnd, LONG lHeight)
{
	HDC hWndDC = GetDC(hWnd);
	TEXTMETRIC tmTextMetric;
	GetTextMetrics(hWndDC, &tmTextMetric);
	ReleaseDC(hWnd, hWndDC);

	LOGFONT lFont;
	memset(&lFont, 0, sizeof(LOGFONT));
	lFont.lfHeight = lHeight;
	lFont.lfWidth = 0;
	lFont.lfOrientation = 0;
	lFont.lfEscapement = 0;
	lFont.lfWeight = FW_DONTCARE;
	lFont.lfUnderline = FALSE;
	lFont.lfStrikeOut = FALSE;
	lFont.lfCharSet = tmTextMetric.tmCharSet;
	lFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lFont.lfQuality = DEFAULT_QUALITY;

	return CreateFontIndirect(&lFont);
}

BOOL PlaceSymbolByPoint(HWND hWnd, COORD cCenterPoint, LONG lFontHeight, LONG lPlaceholderWidth, TCHAR cSymbol, WORD wAngle)
{
	HDC hWndDC = GetDC(hWnd);

	RECT rTextRect;
	rTextRect.top = cCenterPoint.Y - lFontHeight / 2;
	rTextRect.bottom = cCenterPoint.Y + lFontHeight / 2;
	rTextRect.left = cCenterPoint.X - lPlaceholderWidth / 2;
	rTextRect.right = cCenterPoint.X + lPlaceholderWidth / 2;

	HFONT hFont = CreateFontByHeight(hWnd, lFontHeight);
	HGDIOBJ oldObject = SelectObject(hWndDC, hFont);

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

	LPTSTR lpsTextToDisplay = CreateStringByChar(cSymbol);
	BOOL bResult = DrawText(hWndDC, lpsTextToDisplay, 1, &rTextRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	free(lpsTextToDisplay);

	SelectObject(hWndDC, oldObject);
	DeleteObject(hFont);

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

int PlaceTextByRectangleUpperSide(HWND hWnd, COORD cTextCoordinate, SIZE sTextRectSize, LONG lFontHeight, LPTSTR lpsText, int iFirstChar, int iLastChar)
{
	LONG lLetterWidth = sTextRectSize.cx / (iLastChar - iFirstChar + 1);
	COORD cCenterCoordinate;
	cCenterCoordinate.X = (SHORT)(cTextCoordinate.X + lLetterWidth / 2);
	cCenterCoordinate.Y = cTextCoordinate.Y;
	int iResult = 0;

	for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
	{
		iResult += (PlaceSymbolByPoint(hWnd, cCenterCoordinate, lFontHeight, lLetterWidth, lpsText[iLetterCounter], 0) ? 1 : 0);
		cCenterCoordinate.X += (SHORT)lLetterWidth;
	}

	return iResult;
}

int PlaceTextByRectangleRightSide(HWND hWnd, COORD cTextCoordinate, SIZE sTextRectSize, LONG lFontHeight, LPTSTR lpsText, int iFirstChar, int iLastChar)
{
	LONG lLetterWidth = sTextRectSize.cy / (iLastChar - iFirstChar + 1);
	COORD cCenterCoordinate;
	cCenterCoordinate.X = (SHORT)(cTextCoordinate.X + sTextRectSize.cx);
	cCenterCoordinate.Y = (SHORT)(cTextCoordinate.Y + lLetterWidth / 2);
	int iResult = 0;

	for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
	{
		iResult += (PlaceSymbolByPoint(hWnd, cCenterCoordinate, lFontHeight, lLetterWidth, lpsText[iLetterCounter], 90) ? 1 : 0);
		cCenterCoordinate.Y += (SHORT)lLetterWidth;
	}

	return iResult;
}

int PlaceTextByRectangleLowerSide(HWND hWnd, COORD cTextCoordinate, SIZE sTextRectSize, LONG lFontHeight, LPTSTR lpsText, int iFirstChar, int iLastChar)
{
	LONG lLetterWidth = sTextRectSize.cx / (iLastChar - iFirstChar + 1);
	COORD cCenterCoordinate;
	cCenterCoordinate.X = (SHORT)(cTextCoordinate.X + sTextRectSize.cx - lLetterWidth / 2);
	cCenterCoordinate.Y = (SHORT)(cTextCoordinate.Y + sTextRectSize.cy);
	int iResult = 0;

	for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
	{
		iResult += (PlaceSymbolByPoint(hWnd, cCenterCoordinate, lFontHeight, lLetterWidth, lpsText[iLetterCounter], 180) ? 1 : 0);
		cCenterCoordinate.X -= (SHORT)lLetterWidth;
	}

	return iResult;
}

int PlaceTextByRectangleLeftSide(HWND hWnd, COORD cTextCoordinate, SIZE sTextRectSize, LONG lFontHeight, LPTSTR lpsText, int iFirstChar, int iLastChar)
{
	LONG lLetterWidth = sTextRectSize.cy / (iLastChar - iFirstChar + 1);
	COORD cCenterCoordinate;
	cCenterCoordinate.X = cTextCoordinate.X;
	cCenterCoordinate.Y = (SHORT)(cTextCoordinate.Y + sTextRectSize.cy - lLetterWidth / 2);
	int iResult = 0;

	for (int iLetterCounter = iFirstChar; iLetterCounter <= iLastChar; iLetterCounter++)
	{
		iResult += (PlaceSymbolByPoint(hWnd, cCenterCoordinate, lFontHeight, lLetterWidth, lpsText[iLetterCounter], 270) ? 1 : 0);
		cCenterCoordinate.Y -= (SHORT)(lLetterWidth);
	}

	return iResult;
}

int PlaceTextByRectangle(HWND hWnd, COORD cTextCoordinate, SIZE sTextRectSize, LONG lFontHeight, LPTSTR lpsText)
{
	int aiLettersOnSide[4];
	GetLettersCountForRectangleSides(lstrlen(lpsText), aiLettersOnSide);
	int iFirstLetter, iLastLetter;
	int iResult = 0;

	if (aiLettersOnSide[0] > 0)
	{
		iFirstLetter = 0;
		iLastLetter = aiLettersOnSide[0] - 1;
		iResult += PlaceTextByRectangleUpperSide(hWnd, cTextCoordinate, sTextRectSize, lFontHeight, lpsText, iFirstLetter, iLastLetter);
	}

	if (aiLettersOnSide[1] > 0)
	{
		iFirstLetter = aiLettersOnSide[0];
		iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] - 1;
		iResult += PlaceTextByRectangleRightSide(hWnd, cTextCoordinate, sTextRectSize, lFontHeight, lpsText, iFirstLetter, iLastLetter);
	}

	if (aiLettersOnSide[2] > 0)
	{
		iFirstLetter = aiLettersOnSide[0] + aiLettersOnSide[1];
		iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2] - 1;
		iResult += PlaceTextByRectangleLowerSide(hWnd, cTextCoordinate, sTextRectSize, lFontHeight, lpsText, iFirstLetter, iLastLetter);
	}

	if (aiLettersOnSide[3] > 0)
	{
		iFirstLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2];
		iLastLetter = aiLettersOnSide[0] + aiLettersOnSide[1] + aiLettersOnSide[2] + aiLettersOnSide[3] - 1;
		iResult += PlaceTextByRectangleLeftSide(hWnd, cTextCoordinate, sTextRectSize, lFontHeight, lpsText, iFirstLetter, iLastLetter);
	}

	return iResult;
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
	static LONG lFontHeight = GetFontHeight(hWnd);

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
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F5:
			if (lFontHeight > 1)
			{
				--lFontHeight;
				PostDrawStampMessage(hWnd);
			}
			break;
		case VK_F6:
			++lFontHeight;
			PostDrawStampMessage(hWnd);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_DRAW_STAMP:
		FillWindowWithColor(hWnd, GetBackgroundColor());
		PlaceTextByRectangle(hWnd, cStampCoordinates, sStampSize, lFontHeight, lpsText);
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
