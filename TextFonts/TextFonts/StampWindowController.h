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
		LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		void ClearDrawers();
		BOOL AddDrawer(StampDrawer *sStampDrawer);
		size_t NextDrawer();
		size_t GetDrawersCount();
		StampWindowController();
		~StampWindowController();
	private:
		BOOL PostDrawStampMessage(HWND hWnd);
		BOOL PostSizeMessage(HWND hWnd);
		LoadResult LoadStampBackground(HWND hWnd);
		static const UINT WM_DRAW_STAMP;
		StampDrawer **m_asStampDrawers;
		size_t m_uDrawersCount;
		size_t m_uCurDrawer;
	};
}
