#include "Validator_def.h"
//Class:Validator

@+ bool Validator_validateRange(Validator* this, int val)
{
	return pRange->min <= val && val <= pRange->max;
}

