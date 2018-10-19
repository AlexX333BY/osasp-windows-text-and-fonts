#include <strsafe.h>
#include "StringProcessor.h"

namespace Stamp
{
	LPTSTR StringProcessor::CopyString(LPTSTR lpsOriginalString)
	{
		if (lpsOriginalString == NULL)
		{
			return NULL;
		}
		int iStringCopyLength = lstrlen(lpsOriginalString);
		LPTSTR lpsStringCopy = (LPTSTR)calloc(iStringCopyLength + 1, sizeof(TCHAR));
		if (StringCchCopy(lpsStringCopy, iStringCopyLength + 1, lpsOriginalString) == S_OK)
		{
			return lpsStringCopy;
		}
		else
		{
			return NULL;
		}
	}

	LPTSTR StringProcessor::GetEmptyString()
	{
		LPTSTR lpsEmptyString = (LPTSTR)calloc(1, sizeof(TCHAR));
		lpsEmptyString[0] = '\0';
		return lpsEmptyString;
	}

	LPTSTR StringProcessor::DeleteLastChar(LPTSTR lpsOriginalString)
	{
		int iStringLength = lstrlen(lpsOriginalString);
		if (iStringLength < 2) 
		{
			return GetEmptyString();
		}
		else
		{
			LPTSTR lpsNewString = (LPTSTR)calloc(iStringLength, sizeof(TCHAR));
			if (StringCchCopyN(lpsNewString, iStringLength, lpsOriginalString, iStringLength - 1) == S_OK)
			{
				return lpsNewString;
			}
			else
			{
				free(lpsNewString);
				return NULL;
			}
		}
	}

	LPTSTR StringProcessor::AddChar(LPTSTR lpsOriginalString, TCHAR cCharToAdd)
	{
		int iNewLength = lstrlen(lpsOriginalString) + 2;
		LPTSTR lpsNewString = (LPTSTR)calloc(iNewLength, sizeof(TCHAR));
		if (StringCchCopy(lpsNewString, iNewLength, lpsOriginalString) == S_OK)
		{
			lpsNewString[iNewLength - 2] = cCharToAdd;
			lpsNewString[iNewLength - 1] = '\0';
			return lpsNewString;
		}
		else
		{
			free(lpsNewString);
			return NULL;
		}
	}

	LPTSTR StringProcessor::CreateStringByChar(TCHAR cChar)
	{
		LPTSTR lpsCharString = (LPTSTR)calloc(2, sizeof(TCHAR));
		lpsCharString[0] = cChar;
		lpsCharString[1] = '\0';
		return lpsCharString;
	}
}
