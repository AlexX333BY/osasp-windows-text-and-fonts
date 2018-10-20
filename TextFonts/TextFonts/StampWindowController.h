#pragma once

#include "StampDrawer.h"
#include <windows.h>

namespace Stamp
{
	typedef enum LoadResult
	{
		LR_OK = 0,
		LR_CANCELLED_BY_USER = 1,
		LR_ERROR = -1,
	} _LoadResult;

	class StampWindowController
	{
	public:
		LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
		COLORREF GetBackgroundColor();
		void SetBackgroundColor(COLORREF crBackgroundColor);
		static COLORREF GetDefaultBackgroundColor();
		StampWindowController(HWND hWnd);
		StampWindowController(HWND hWnd, COLORREF crBackgroundColor);
		~StampWindowController();
	private:
		BOOL PostDrawStampMessage();
		BOOL PostSizeMessage();
		LoadResult LoadStampBackground();
		static const UINT WM_DRAW_STAMP;
		HWND m_hWnd;
		StampDrawer *m_sStampDrawer;
		COLORREF m_crBackgroundColor;
	};
}
