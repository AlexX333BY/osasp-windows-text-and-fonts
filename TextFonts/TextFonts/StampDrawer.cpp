#include "StampDrawer.h"
#include "StringProcessor.h"
#include <gdiplus.h>
#include <strsafe.h>

namespace Stamp
{
	BOOL StampDrawer::LoadBackgroundImage(char psFileName[])
	{
		int iFileNameLength = MultiByteToWideChar(CP_ACP, 0, psFileName, -1, NULL, 0);
		WCHAR *cWideCharFileName = new WCHAR[MultiByteToWideChar(CP_ACP, 0, psFileName, -1, NULL, 0)];
		MultiByteToWideChar(CP_ACP, 0, psFileName, -1, cWideCharFileName, iFileNameLength);

		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR ulpGdiplusToken;
		GdiplusStartup(&ulpGdiplusToken, &gdiplusStartupInput, NULL);

		Gdiplus::Bitmap *sourceImage = Gdiplus::Bitmap::FromFile(cWideCharFileName);
		HBITMAP hNewBitmap;
		Gdiplus::Color imageBackgroundColor;
		imageBackgroundColor.SetFromCOLORREF(m_dwImageBackgroundColor);
		Gdiplus::Status bitmapStatus = sourceImage->GetHBITMAP(imageBackgroundColor, &hNewBitmap);

		Gdiplus::GdiplusShutdown(ulpGdiplusToken);
		delete cWideCharFileName;

		if (bitmapStatus != Gdiplus::Ok)
		{
			return FALSE;
		}

		if (m_hBackgroundImage != NULL)
		{
			DeleteObject(m_hBackgroundImage);
		}

		m_hBackgroundImage = hNewBitmap;
		m_bIsBackgroundInherited = FALSE;
		return TRUE;
	}

	BOOL StampDrawer::DeleteBackgroundImage()
	{
		if ((m_hBackgroundImage == NULL) || m_bIsBackgroundInherited)
		{
			return TRUE;
		}
		if (!DeleteObject(m_hBackgroundImage))
		{
			return FALSE;
		}
		return Draw();
	}

	LPTSTR StampDrawer::GetText()
	{
		return StringProcessor::CopyString(m_lpsText);
	}

	BOOL StampDrawer::SetText(LPTSTR lpsNewText)
	{
		if (lpsNewText == NULL)
		{
			return FALSE;
		}
		else
		{
			LPTSTR lpsCopiedText = StringProcessor::CopyString(lpsNewText);
			if (lpsCopiedText == NULL)
			{
				return FALSE;
			}
			else
			{
				free(m_lpsText);
				m_lpsText = lpsCopiedText;
				return TRUE;
			}
		}
	}

	LONG StampDrawer::GetFontHeight()
	{
		return m_lFontHeight;
	}

	BOOL StampDrawer::SetFontHeight(LONG lFontHeight)
	{
		if ((lFontHeight >= 0) && (lFontHeight <= GetMaxFontHeight()))
		{
			m_lFontHeight = lFontHeight;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL StampDrawer::IncrementFontSize()
	{
		return SetFontHeight(m_lFontHeight + 1);
	}

	BOOL StampDrawer::DecrementFontSize()
	{
		return SetFontHeight(m_lFontHeight - 1);
	}

	StampIndent StampDrawer::GetStampIndent()
	{
		return m_sStampIndent;
	}
	
	BOOL StampDrawer::SetStampIndent(StampIndent sStampIndent)
	{
		if (!IsBetween(sStampIndent.m_cbLeft, 0, 100)
			|| !IsBetween(sStampIndent.m_cbRight, 0, 100)
			|| !IsBetween(sStampIndent.m_cbTop, 0, 100)
			|| !IsBetween(sStampIndent.m_cbBottom, 0, 100)
			|| (sStampIndent.m_cbLeft + sStampIndent.m_cbRight >= 100)
			|| (sStampIndent.m_cbTop + sStampIndent.m_cbBottom >= 100))
		{
			return FALSE;
		}

		m_sStampIndent = sStampIndent;
		LONG lMaxFontHeight = GetMaxFontHeight();
		if (m_lFontHeight > lMaxFontHeight)
		{
			SetFontHeight(lMaxFontHeight);
		}
		return TRUE;
	}

	BOOL StampDrawer::IncrementStampIndent()
	{
		StampIndent sNewStampIndent = m_sStampIndent;
		++sNewStampIndent.m_cbBottom;
		++sNewStampIndent.m_cbLeft;
		++sNewStampIndent.m_cbRight;
		++sNewStampIndent.m_cbTop;
		return SetStampIndent(sNewStampIndent);
	}

	BOOL StampDrawer::DecrementStampIndent()
	{
		StampIndent sNewStampIndent = m_sStampIndent;
		--sNewStampIndent.m_cbBottom;
		--sNewStampIndent.m_cbLeft;
		--sNewStampIndent.m_cbRight;
		--sNewStampIndent.m_cbTop;
		return SetStampIndent(sNewStampIndent);
	}

	BOOL StampDrawer::IsBetween(INT64 iValue, INT64 iLeftBound, INT64 iRightBound)
	{
		return (iValue >= iLeftBound) && (iValue <= iRightBound);
	}

	LONG StampDrawer::GetWindowFontHeight(HWND hWnd)
	{
		HDC hWndDC = GetDC(hWnd);
		TEXTMETRIC tmTextMetric;
		GetTextMetrics(hWndDC, &tmTextMetric);
		ReleaseDC(hWnd, hWndDC);
		return tmTextMetric.tmHeight;
	}

	SIZE StampDrawer::GetWindowSize(HWND hWnd)
	{
		RECT rWndRect;
		GetClientRect(hWnd, &rWndRect);
		SIZE sWndSize;
		sWndSize.cx = rWndRect.right - rWndRect.left;
		sWndSize.cy = rWndRect.bottom - rWndRect.top;
		return sWndSize;
	}

	LONG StampDrawer::GetMaxFontHeight()
	{
		SIZE sWindowSize = GetWindowSize(m_hWnd);
		return min(min(m_cStampCoordinates.X, m_cStampCoordinates.Y), 
			min(sWindowSize.cx - m_cStampCoordinates.X - m_sStampSize.cx, 
				sWindowSize.cy - m_cStampCoordinates.Y - m_sStampSize.cy));
	}

	StampDrawer::StampDrawer(HWND hWnd, COLORREF crImageBackgroundColor) 
		: m_hWnd(hWnd), m_dwImageBackgroundColor(crImageBackgroundColor), 
		m_hBackgroundImage(NULL), m_lpsText(StringProcessor::GetEmptyString()), 
		m_lFontHeight(GetWindowFontHeight(hWnd))
	{ }

	StampDrawer::~StampDrawer()
	{
		if (!m_bIsBackgroundInherited)
		{
			DeleteObject(m_hBackgroundImage);
		}
		free(m_lpsText);
	}
}
