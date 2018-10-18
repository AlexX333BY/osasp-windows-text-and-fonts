#pragma once

#include <windows.h>

namespace Stamp
{
	struct StampIndent
	{
	public:
		BYTE m_cbTop, m_cbRight, m_cbBottom, m_cbLeft;
		StampIndent();
	};
}