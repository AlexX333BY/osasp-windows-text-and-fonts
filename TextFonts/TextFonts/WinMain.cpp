#include <windows.h>
#include <strsafe.h>

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
		return false;
	}

	SIZE sWndSize = GetClientWindowSize(hWnd);
	rcStampCoords.X = (SHORT)(sWndSize.cx * cbLeftIndentPercents / 100);
	rcStampCoords.Y = (SHORT)(sWndSize.cy * cbUpIndentPercents / 100);
	rsStampSize.cx = (SHORT)(sWndSize.cx * (100 - cbLeftIndentPercents 
		- cbRightIndentPercents) / 100);
	rsStampSize.cy = (SHORT)(sWndSize.cy * (100 - cbDownIndentPercents 
		- cbUpIndentPercents) / 100);
	return true;
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static COORD cStampCoordinates = { 0 };
	static SIZE sStampSize = { 0 };
	static LPTSTR lpsText = GetEmptyString();

	switch (message)
	{
	case WM_SIZE:
		UpdateStampPosition(hWnd, cStampCoordinates, sStampSize);
		DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
		FillWindowWithColor(hWnd, GetBackgroundColor());
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
			}
		case VK_RETURN:
		case VK_ESCAPE:
			break;
		default:
			if ((lpsNewText = ConcatStringAndChar(lpsText, (TCHAR)wParam)) != NULL)
			{
				free(lpsText);
				lpsText = lpsNewText;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
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
