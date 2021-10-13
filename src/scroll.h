#pragma once
#include "framework.h"


extern int g_lSensitivityX;
extern int g_lSensitivityY;

BOOL fn_bInitScroll( void );
void fn_vDeInitScroll( void );

BOOL fn_bSetSensitivityYFromStr( char const *szStr );
BOOL fn_bSetSensitivityXFromStr( char const *szStr );

void fn_vSaveConfig( void );
void fn_vLoadConfig( void );