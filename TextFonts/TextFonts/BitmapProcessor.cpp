#include "BitmapProcessor.h"

namespace Stamp
{
	SIZE BitmapProcessor::GetBitmapSize(HBITMAP hBitmap)
	{
		BITMAP bBitmap;
		GetObject(hBitmap, sizeof(BITMAP), &bBitmap);
		SIZE sResult;
		sResult.cx = bBitmap.bmWidth;
		sResult.cy = bBitmap.bmHeight;
		return sResult;
	}
}
