#pragma once

#include <windows.h>

namespace Stamp
{
	static class XformCreator
	{
	public:
		static XFORM CreateRotationXform(WORD wDegreeAngle);
		static XFORM CreateMovementXform(COORD cCoord);
	};
}
