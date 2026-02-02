#pragma once
#include "framework.h"


extern int g_lSensitivityX;
extern int g_lSensitivityY;
extern BOOL g_bReverse;
extern BOOL g_bSmooth;

BOOL fn_bInitScroll( void );
void fn_vDeInitScroll( void );

void fn_vSaveConfig( void );
void fn_vLoadConfig( void );
void fn_vUpdateConfig( void );
