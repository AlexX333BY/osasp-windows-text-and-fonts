#include "StampDrawer.h"
#include "StringProcessor.h"
#include "WindowProcessor.h"
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
		imageBackgroundColor.SetFromCOLORREF(m_crImageBackgroundColor);
		Gdiplus::Status bitmapStatus = sourceImage->GetHBITMAP(imageBackgroundColor, &hNewBitmap);

		Gdiplus::GdiplusShutdown(ulpGdiplusToken);
		delete cWideCharFileName;

		if (bitmapStatus != Gdiplus::Ok)
		{
			return FALSE;
		}

		DeleteBackgroundImage();

		m_hBackgroundImage = hNewBitmap;
		return TRUE;
	}

	BOOL StampDrawer::DeleteBackgroundImage()
	{
		if (m_hBackgroundImage == NULL)
		{
			return TRUE;
		}
		BOOL bResult = DeleteObject(m_hBackgroundImage);
		m_hBackgroundImage = NULL;
		return bResult;
	}

	BOOL StampDrawer::HasStampImage()
	{
		return m_hBackgroundImage != NULL;
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

	BOOL StampDrawer::AddSymbol(TCHAR cSymbol)
	{
		LPTSTR lpsNewText = StringProcessor::AddChar(m_lpsText, cSymbol);
		if (lpsNewText == NULL)
		{
			return FALSE;
		}
		else
		{
			free(m_lpsText);
			m_lpsText = lpsNewText;
			return TRUE;
		}
	}

	BOOL StampDrawer::DeleteLastSymbol()
	{
		LPTSTR lpsNewText = StringProcessor::DeleteLastChar(m_lpsText);
		if (lpsNewText == NULL)
		{
			return FALSE;
		}
		else
		{
			free(m_lpsText);
			m_lpsText = lpsNewText;
			return TRUE;
		}
	}

	int StampDrawer::GetTextLength()
	{
		return lstrlen(m_lpsText);
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

	COLORREF StampDrawer::GetImageBackgroundColor()
	{
		return m_crImageBackgroundColor;
	}

	void StampDrawer::SetImageBackgroundColor(COLORREF crNewColor)
	{
		m_crImageBackgroundColor = crNewColor;
	}

	BOOL StampDrawer::SetByAnother(StampDrawer *sStampDrawer)
	{
		if (sStampDrawer == NULL)
		{
			return FALSE;
		}

		BOOL bResult = TRUE;

		DeleteBackgroundImage();
		if (sStampDrawer->m_hBackgroundImage == NULL)
		{
			m_hBackgroundImage = NULL;
		}
		else
		{
			m_hBackgroundImage = (HBITMAP)CopyImage(sStampDrawer->m_hBackgroundImage, IMAGE_BITMAP, 0, 0, 0);
			bResult &= (m_hBackgroundImage == NULL);
		}

		m_crImageBackgroundColor = sStampDrawer->GetImageBackgroundColor();
		
		LPTSTR lpsText = sStampDrawer->GetText();
		if (lpsText == NULL)
		{
			bResult &= FALSE;
		}
		else
		{
			free(m_lpsText);
			m_lpsText = lpsText;
		}

		bResult &= SetStampIndent(sStampDrawer->GetStampIndent());
		UpdateStampSize();

		if (!SetFontHeight(sStampDrawer->m_lFontHeight))
		{
			bResult &= SetFontHeight(GetMaxFontHeight());
		}
		return bResult;
	}

	void StampDrawer::UpdateStampSize()
	{
		SIZE sWndSize = WindowProcessor::GetWindowSize(m_hWnd);
		m_cStampCoordinates.X = (SHORT)(sWndSize.cx * m_sStampIndent.m_cbLeft / 100);
		m_cStampCoordinates.Y = (SHORT)(sWndSize.cy * m_sStampIndent.m_cbTop / 100);
		m_sStampSize.cx = (SHORT)(sWndSize.cx * (100 - m_sStampIndent.m_cbLeft
			- m_sStampIndent.m_cbRight) / 100);
		m_sStampSize.cy = (SHORT)(sWndSize.cy * (100 - m_sStampIndent.m_cbBottom
			- m_sStampIndent.m_cbTop) / 100);

		LONG lMaxFontHeight = GetMaxFontHeight();
		if (lMaxFontHeight < m_lFontHeight)
		{
			SetFontHeight(lMaxFontHeight);
		}
	}

	BOOL StampDrawer::IsBetween(INT64 iValue, INT64 iLeftBound, INT64 iRightBound)
	{
		return (iValue >= iLeftBound) && (iValue <= iRightBound);
	}

	LONG StampDrawer::GetMaxFontHeight()
	{
		SIZE sWindowSize = WindowProcessor::GetWindowSize(m_hWnd);
		return min(min(m_cStampCoordinates.X, m_cStampCoordinates.Y), 
			min(sWindowSize.cx - m_cStampCoordinates.X - m_sStampSize.cx, 
				sWindowSize.cy - m_cStampCoordinates.Y - m_sStampSize.cy));
	}

	StampDrawer::StampDrawer(HWND hWnd, COLORREF crImageBackgroundColor)
		: m_hWnd(hWnd), m_crImageBackgroundColor(crImageBackgroundColor), 
		m_hBackgroundImage(NULL), m_lpsText(StringProcessor::GetEmptyString())
	{
		StampIndent sStampIndent;
		m_sStampIndent = sStampIndent;
		UpdateStampSize();
		if (!SetFontHeight(WindowProcessor::GetWindowFontHeight(hWnd)))
		{
			SetFontHeight(GetMaxFontHeight());
		}
	}

	StampDrawer::~StampDrawer()
	{
		DeleteBackgroundImage();
		free(m_lpsText);
	}
}
