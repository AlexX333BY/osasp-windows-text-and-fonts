#include "FontProcessor.h"

namespace Stamp
{
	HFONT FontProcessor::CreateFontByHeight(HWND hWnd, LONG lHeight)
	{
		HDC hWndDC = GetDC(hWnd);
		TEXTMETRIC tmTextMetric;
		GetTextMetrics(hWndDC, &tmTextMetric);
		ReleaseDC(hWnd, hWndDC);

		LOGFONT lFont;
		memset(&lFont, 0, sizeof(LOGFONT));
		lFont.lfHeight = lHeight;
		lFont.lfWidth = 0;
		lFont.lfOrientation = 0;
		lFont.lfEscapement = 0;
		lFont.lfWeight = FW_DONTCARE;
		lFont.lfUnderline = FALSE;
		lFont.lfStrikeOut = FALSE;
		lFont.lfCharSet = tmTextMetric.tmCharSet;
		lFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lFont.lfQuality = DEFAULT_QUALITY;

		return CreateFontIndirect(&lFont);
	}
}
