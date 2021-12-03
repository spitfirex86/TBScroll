#pragma once
#include "framework.h"


#define ABS(x) (((x) >= 0) ? (x) : -(x))

BOOL fn_bIntFromStr( char const *szIn, int *p_lOut );
int fn_lReadIntFromCfg( char const *szKey, int lDefault );
void fn_vWriteIntToCfg( char const *szKey, int lValue );
