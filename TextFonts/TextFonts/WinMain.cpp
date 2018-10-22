#include <windows.h>
#include "StampWindowController.h"
#include "RectangleStampDrawer.h"
#include "EllipseStampDrawer.h"
#include "WindowProcessor.h"

COLORREF crBackgroundColor;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static Stamp::StampWindowController *sStampWindowController = new Stamp::StampWindowController();

	if (message == WM_CREATE)
	{
		sStampWindowController->AddDrawer(new Stamp::RectangleStampDrawer(hWnd, crBackgroundColor));
		sStampWindowController->AddDrawer(new Stamp::EllipseStampDrawer(hWnd, crBackgroundColor));
	}

	return sStampWindowController->HandleMessage(hWnd, message, wParam, lParam);
}

COLORREF GetUserColor()
{
	CHOOSECOLOR cColorWindowStruct;
	COLORREF acrCustomColors[16] = { 0 };
	cColorWindowStruct.lStructSize = sizeof(CHOOSECOLOR);
	cColorWindowStruct.hwndOwner = NULL;
	cColorWindowStruct.rgbResult = Stamp::WindowProcessor::GetDefaultBackgroundColor();
	cColorWindowStruct.lpCustColors = acrCustomColors;
	cColorWindowStruct.Flags = CC_RGBINIT | CC_SOLIDCOLOR;
	ChooseColor(&cColorWindowStruct);
	return cColorWindowStruct.rgbResult;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPTSTR lpCmdLine, int nCmdShow)
{
	crBackgroundColor = GetUserColor();

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
	wndClassEx.hbrBackground = CreateSolidBrush(crBackgroundColor);
	wndClassEx.lpszMenuName = NULL;
	wndClassEx.lpszClassName = spWndClassName;
	wndClassEx.hIconSm = NULL;
	RegisterClassEx(&wndClassEx);

	HWND hWnd = CreateWindow(spWndClassName, "Task #2", WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, 
		hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
