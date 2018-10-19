#pragma once

#include <windows.h>

namespace Stamp
{
	static class StringProcessor
	{
	public:
		static LPTSTR CopyString(LPTSTR lpsOriginalString);
		static LPTSTR GetEmptyString();
		static LPTSTR DeleteLastChar(LPTSTR lpsOriginalString);
		static LPTSTR AddChar(LPTSTR lpsOriginalString, TCHAR cCharToAdd);
		static LPTSTR CreateStringByChar(TCHAR cChar);
	};
}
