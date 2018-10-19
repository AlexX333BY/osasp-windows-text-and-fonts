#include <windows.h>
#include "StampDrawer.h"
#include "RectangleStampDrawer.h"

#define _USE_MATH_DEFINES 

#include <cmath>

using namespace Stamp;

#define WM_DRAW_STAMP WM_USER

DWORD GetBackgroundColor()
{
	return GetSysColor(COLOR_WINDOW);
}

BOOL PostDrawStampMessage(HWND hWnd)
{
	return PostMessage(hWnd, WM_DRAW_STAMP, NULL, NULL);
}

BOOL PostWmSizeMessage(HWND hWnd)
{
	return PostMessage(hWnd, WM_SIZE, NULL, NULL);
}

typedef enum LoadResult
{
	LR_OK = 0,
	LR_CANCELLED_BY_USER = 1,
	LR_ERROR = -1,
} _LoadResult;

LoadResult LoadStampBackground(HWND hWnd, StampDrawer *sStampDrawer)
{
	if (sStampDrawer == NULL)
	{
		return LR_ERROR;
	}

	char psFileName[MAX_PATH] = { '\0' };

	OPENFILENAME oOpenFileName;
	oOpenFileName.lStructSize = sizeof(OPENFILENAME);
	oOpenFileName.hwndOwner = hWnd;
	oOpenFileName.hInstance = NULL;
	oOpenFileName.lpstrFilter = "Images\0*.bmp;*.gif;*.jpeg;*.png;*.tiff;*.exif;*.wmf;*.emf;*.jpg\0\0";
	oOpenFileName.lpstrCustomFilter = NULL;
	oOpenFileName.nFilterIndex = 1;
	oOpenFileName.lpstrFile = psFileName;
	oOpenFileName.nMaxFile = sizeof(psFileName);
	oOpenFileName.lpstrFileTitle = NULL;
	oOpenFileName.lpstrInitialDir = NULL;
	oOpenFileName.lpstrTitle = "Select stamp background";
	oOpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	oOpenFileName.lpstrDefExt = NULL;

	if (GetOpenFileName(&oOpenFileName))
	{
		BOOL bLoadResult = sStampDrawer->LoadBackgroundImage(psFileName);
		free(psFileName);
		if (bLoadResult)
		{
			return LR_OK;
		}
		else
		{
			return LR_ERROR;
		}
	}
	return LR_CANCELLED_BY_USER;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	StampDrawer *sStampDrawer = new RectangleStampDrawer(hWnd, GetBackgroundColor());

	switch (message)
	{
	case WM_SIZE:
		sStampDrawer->UpdateStampSize();
		PostDrawStampMessage(hWnd);
		DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_CHAR:
		LPTSTR lpsNewText;
		switch (wParam)
		{
		case VK_BACK:
			if (sStampDrawer->GetTextLength() > 0)
			{
				sStampDrawer->DeleteLastSymbol();
				PostDrawStampMessage(hWnd);
			}
			break;
		case VK_RETURN:
		case VK_ESCAPE:
			break;
		default:
			sStampDrawer->AddSymbol((TCHAR)wParam);
			PostDrawStampMessage(hWnd);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F2:
			if (LoadStampBackground(hWnd, sStampDrawer) == LR_OK)
			{
				PostDrawStampMessage(hWnd);
			}
			break;
		case VK_F3:
			if (sStampDrawer->HasStampImage())
			{
				sStampDrawer->DeleteBackgroundImage();
				PostDrawStampMessage(hWnd);
			}
			break;
		}
		break;
	case WM_MOUSEWHEEL:
		if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
		{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				if (sStampDrawer->IncrementStampIndent())
				{
					PostWmSizeMessage(hWnd);
				}
			}
			else
			{
				if (sStampDrawer->DecrementStampIndent())
				{
					PostWmSizeMessage(hWnd);
				}
			}
		}
		else
		{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				if (sStampDrawer->IncrementFontSize())
				{
					PostDrawStampMessage(hWnd);
				}
			}
			else
			{
				if (sStampDrawer->DecrementFontSize())
				{
					PostDrawStampMessage(hWnd);
				}
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_DRAW_STAMP:
		sStampDrawer->Draw();
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
