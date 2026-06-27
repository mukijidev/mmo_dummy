#pragma once
#include "Type.h"


#include <cmath>


namespace Util
{
	double CalculateRotation(const FVector& oldPosition, const FVector& newPosition);
	const char* FieldName(uint16 fieldId);
}




