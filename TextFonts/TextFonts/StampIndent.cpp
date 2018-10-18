#include "StampIndent.h"

namespace Stamp
{
	StampIndent StampIndent::GetDefault()
	{
		StampIndent sStampIndent;
		sStampIndent.cbBottom = 10;
		sStampIndent.cbLeft = 10;
		sStampIndent.cbRight = 10;
		sStampIndent.cbTop = 10;
		return sStampIndent;
	}
}