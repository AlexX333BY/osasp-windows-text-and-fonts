#include "XformCreator.h"

#define _USE_MATH_DEFINES 

#include <cmath>

namespace Stamp
{
	XFORM XformCreator::CreateRotationXform(DOUBLE dDegreeAngle)
	{
		XFORM xForm;
		FLOAT fRadAngle = (FLOAT)(M_PI * dDegreeAngle / 180);
		FLOAT fAngleSin = sin(fRadAngle);
		FLOAT fAngleCos = cos(fRadAngle);
		xForm.eM11 = fAngleCos;
		xForm.eM12 = fAngleSin;
		xForm.eM21 = -fAngleSin;
		xForm.eM22 = fAngleCos;
		xForm.eDx = 0;
		xForm.eDy = 0;
		return xForm;
	}

	XFORM XformCreator::CreateMovementXform(COORD cCoordinates)
	{
		XFORM xForm;
		xForm.eM11 = 1;
		xForm.eM12 = 0;
		xForm.eM21 = 0;
		xForm.eM22 = 1;
		xForm.eDx = cCoordinates.X;
		xForm.eDy = cCoordinates.Y;
		return xForm;
	}
}
