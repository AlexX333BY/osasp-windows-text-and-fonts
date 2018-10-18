#pragma once

#include <windows.h>

namespace Stamp
{
	struct StampIndent
	{
	public:
		BYTE cbTop, cbRight, cbBottom, cbLeft;
		static StampIndent GetDefault();
	};
}