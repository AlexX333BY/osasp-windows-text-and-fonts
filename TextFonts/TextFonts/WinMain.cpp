#include <windows.h>
#include "StampWindowController.h"
#include "RectangleStampDrawer.h"
#include "WindowProcessor.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static Stamp::StampWindowController *sStampWindowController = new Stamp::StampWindowController();

	if (message == WM_CREATE)
	{
		sStampWindowController->AddDrawer(new Stamp::RectangleStampDrawer(hWnd, Stamp::WindowProcessor::GetDefaultBackgroundColor()));
	}

	return sStampWindowController->HandleMessage(hWnd, message, wParam, lParam);
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
	wndClassEx.hbrBackground = CreateSolidBrush(Stamp::WindowProcessor::GetDefaultBackgroundColor());
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
