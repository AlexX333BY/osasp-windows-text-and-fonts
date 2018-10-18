#pragma once

#include "StampIndent.h"
#include <windows.h>

namespace Stamp
{
	class StampDrawer
	{
	public:
		virtual BOOL Draw() = 0;

		BOOL LoadBackgroundImage(char psFileName[]);
		BOOL DeleteBackgroundImage();

		LPTSTR GetText();
		virtual BOOL SetText(LPTSTR lpsNewText);

		LONG GetFontHeight();
		virtual BOOL SetFontHeight(LONG lFontHeight);
		virtual BOOL IncrementFontSize();
		virtual BOOL DecrementFontSize();

		StampIndent GetStampIndent();
		virtual BOOL SetStampIndent(StampIndent sStampIndent);
		virtual BOOL IncrementStampIndent();
		virtual BOOL DecrementStampIndent();

		virtual BOOL UpdateStampSize();

		BOOL SetByAnother(const StampDrawer *sStampDrawer);

		StampDrawer(HWND hWnd, COLORREF crImageBackgroundColor);
		~StampDrawer();
	protected:
		static BOOL IsBetween(INT64 iValue, INT64 iLeftBound, INT64 iRightBound);
		static LONG GetWindowFontHeight(HWND hWnd);
		static SIZE GetWindowSize(HWND hWnd);
		LONG GetMaxFontHeight();

		HWND m_hWnd;
		HBITMAP m_hBackgroundImage;
		COLORREF m_dwImageBackgroundColor;
		BOOL m_bIsBackgroundInherited;
		LPTSTR m_lpsText;
		LONG m_lFontHeight;
		COORD m_cStampCoordinates;
		StampIndent m_sStampIndent;
		SIZE m_sStampSize;
	};
}
