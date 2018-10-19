#pragma once

#include <windows.h>

namespace Stamp
{
	class XformCreator
	{
	public:
		static XFORM CreateRotationXform(WORD wDegreeAngle);
		static XFORM CreateMovementXform(COORD cCoord);
	};
}
