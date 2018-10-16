#include <windows.h>

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
	RECT rect;
	GetClientRect(hWnd, &rect);
	SIZE size;
	size.cx = rect.right - rect.left;
	size.cy = rect.bottom - rect.top;
	return size;
}

BOOL UpdateStampPosition(HWND hWnd, COORD &rcStampCoords, SIZE &rsStampSize,
	BYTE cbLeftIndentPercents = defaults::cbLeftIndentPercents, BYTE cbRightIndentPercents = defaults::cbRightIndentPercents,
	BYTE cbUpIndentPercents = defaults::cbUpIndentPercents, BYTE cbDownIndentPercents = defaults::cbDownIndentPercents)
{
	if (!IsBetween(cbLeftIndentPercents, 0, 100) || !IsBetween(cbRightIndentPercents, 0, 100)
		|| !IsBetween(cbUpIndentPercents, 0, 100) || !IsBetween(cbDownIndentPercents, 0, 100)
		|| (cbLeftIndentPercents + cbRightIndentPercents >= 100) || (cbDownIndentPercents + cbUpIndentPercents >= 100))
	{
		return false;
	}

	SIZE sizeWnd = GetClientWindowSize(hWnd);
	rcStampCoords.X = (SHORT)(sizeWnd.cx * (cbLeftIndentPercents / 100));
	rcStampCoords.Y = (SHORT)(sizeWnd.cy * (cbUpIndentPercents / 100));
	rsStampSize.cx = (SHORT)(sizeWnd.cx * (100 - cbLeftIndentPercents - cbRightIndentPercents) / 100);
	rsStampSize.cy = (SHORT)(sizeWnd.cy * (100 - cbDownIndentPercents - cbUpIndentPercents) / 100);
	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static COORD coordStamp = { 0 };
	static SIZE sizeStamp = { 0 };

	switch (message)
	{
	case WM_SIZE:
		UpdateStampPosition(hWnd, coordStamp, sizeStamp);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
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

	HWND hWnd = CreateWindow(spWndClassName, "Task #2", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
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
