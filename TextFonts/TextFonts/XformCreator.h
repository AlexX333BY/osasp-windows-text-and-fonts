#pragma once

#include <windows.h>

namespace Stamp
{
	class XformCreator
	{
	public:
		static XFORM CreateRotationXform(DOUBLE dDegreeAngle);
		static XFORM CreateMovementXform(COORD cCoord);
	};
}
