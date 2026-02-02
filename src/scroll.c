#include "scroll.h"
#include "buttons.h"
#include "worker.h"
#include "utils.h"
#include "framework.h"


char const *C_szKeySensitivityY = "VSensitivity";
char const *C_szKeySensitivityX = "HSensitivity";
char const *C_szKeyReverse = "ReverseScroll";
char const *C_szKeySmooth = "SmoothScroll";
char const *C_szKeyButton = "Button";

HHOOK g_hHook = NULL;

int g_lSensitivityX = 120;
int g_lSensitivityY = 20;
BOOL g_bReverse = FALSE;

#define SMOOTH_FACTOR 10
BOOL g_bSmooth = FALSE;
int g_lEffectiveSensX = 12;
int g_lEffectiveSensY = 2;
int g_lEffectiveDelta = WHEEL_DELTA / 10;

BOOL g_bButtonPressed = FALSE;
BOOL g_bScrolling = FALSE;
BOOL g_bEmulatingClick = FALSE;

int g_lStartX = 0;
int g_lStartY = 0;
int g_lMoveX = 0;
int g_lMoveY = 0;


LRESULT CALLBACK fn_lMouseHook( int nCode, WPARAM wParam, LPARAM lParam )
{
	MSLLHOOKSTRUCT *p_stMouse = (MSLLHOOKSTRUCT *)lParam;
	tdstScrollButton const *p_stButton = M_p_stGetCurrentButton();
	tdstScrollButton const *p_stButtonEmulated = M_p_stGetCurrentEmulatedButton();
	
	if ( nCode < 0 )
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);

	if ( wParam == p_stButtonEmulated->ulMessageDown && HIWORD(p_stMouse->mouseData) == p_stButtonEmulated->uwParam )
	{
		// Pass emulated click to other hooks
		if ( g_bEmulatingClick )
			goto j_NextHook;
	}
	if ( wParam == p_stButton->ulMessageDown && HIWORD(p_stMouse->mouseData) == p_stButton->uwParam )
	{
		g_bButtonPressed = TRUE;
		g_bScrolling = FALSE;
		g_lStartX = p_stMouse->pt.x;
		g_lStartY = p_stMouse->pt.y;
		g_lMoveX = 0;
		g_lMoveY = 0;

		return TRUE;
	}

	if ( wParam == p_stButtonEmulated->ulMessageUp && HIWORD(p_stMouse->mouseData) == p_stButtonEmulated->uwParam )
	{
		if ( g_bEmulatingClick )
		{
			// We are done emulating the click, pass to other hooks
			g_bEmulatingClick = FALSE;
			goto j_NextHook;
		}
	}
	if ( wParam == p_stButton->ulMessageUp && HIWORD(p_stMouse->mouseData) == p_stButton->uwParam )
	{
		g_bButtonPressed = FALSE;

		// If scrolling did not occur, emulate click
		if ( !g_bScrolling )
		{
			g_bEmulatingClick = TRUE;
			fn_vQueueClick(g_lStartX, g_lStartY);
		}

		return TRUE;
	}
	else if ( wParam == WM_MOUSEMOVE )
	{
		if ( !g_bButtonPressed )
			goto j_NextHook;

		int lDeltaX = p_stMouse->pt.x - g_lStartX;
		int lDeltaY = p_stMouse->pt.y - g_lStartY;

		g_lMoveX += g_bReverse ? -lDeltaX : lDeltaX;
		g_lMoveY += g_bReverse ? -lDeltaY : lDeltaY;

		// Vertical scroll
		if ( ABS(g_lMoveY) > g_lEffectiveSensY )
		{
			g_bScrolling = TRUE;

			int lSteps = g_lMoveY / g_lEffectiveSensY;
			int lDelta = -lSteps * g_lEffectiveDelta;

			g_lMoveY -= lSteps * g_lEffectiveSensY;

			fn_vQueueVScroll(lDelta);
		}

		// Horizontal scroll
		if ( ABS(g_lMoveX) > g_lEffectiveSensX )
		{
			g_bScrolling = TRUE;

			int lSteps = g_lMoveX / g_lEffectiveSensX;
			int lDelta = lSteps * g_lEffectiveDelta;

			g_lMoveX -= lSteps * g_lEffectiveSensX;
			if ( !g_bSmooth ) // cancel out the other direction in detent mode
				g_lMoveY = 0;

			fn_vQueueHScroll(lDelta);
		}

		return TRUE;
	}

j_NextHook:
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

BOOL fn_bInitScroll( void )
{
	if ( !fn_bStartWorkerThread() )
		return FALSE;

	g_hHook = SetWindowsHookEx(WH_MOUSE_LL, fn_lMouseHook, NULL, 0);
	return TRUE;
}

void fn_vDeInitScroll( void )
{
	UnhookWindowsHookEx(g_hHook);
	fn_bStopWorkerThread();
}

void fn_vSaveConfig( void )
{
	fn_vWriteIntToCfg(C_szKeySensitivityY, g_lSensitivityY);
	fn_vWriteIntToCfg(C_szKeySensitivityX, g_lSensitivityX);

	fn_vWriteIntToCfg(C_szKeyReverse, g_bReverse);
	fn_vWriteIntToCfg(C_szKeySmooth, g_bSmooth);

	fn_vWriteIntToCfg(C_szKeyButton, g_eCurrentButton);
}

void fn_vLoadConfig( void )
{
	g_lSensitivityY = fn_lReadIntFromCfg(C_szKeySensitivityY, 20);
	g_lSensitivityX = fn_lReadIntFromCfg(C_szKeySensitivityX, 120);

	g_bReverse = fn_lReadIntFromCfg(C_szKeyReverse, FALSE);
	g_bSmooth = fn_lReadIntFromCfg(C_szKeySmooth, FALSE);

	g_eCurrentButton = fn_lReadIntFromCfg(C_szKeyButton, C_DefaultScrollButton);
	fn_vUpdateConfig();
}

void fn_vUpdateConfig( void )
{
	if ( g_lSensitivityX <= 0 )
		g_lSensitivityX = INT_MAX;

	if ( g_lSensitivityY <= 0 )
		g_lSensitivityY = INT_MAX;

	if ( g_bSmooth )
	{
		g_lEffectiveSensX = g_lSensitivityX / SMOOTH_FACTOR;
		if ( g_lEffectiveSensX == 0 && g_lSensitivityY > 0 )
			g_lEffectiveSensX = 1;

		g_lEffectiveSensY = g_lSensitivityY / SMOOTH_FACTOR;
		if ( g_lEffectiveSensY == 0 && g_lSensitivityY > 0 )
			g_lEffectiveSensY = 1;

		g_lEffectiveDelta = WHEEL_DELTA / SMOOTH_FACTOR;
	}
	else
	{
		g_lEffectiveSensX = g_lSensitivityX;
		g_lEffectiveSensY = g_lSensitivityY;
		g_lEffectiveDelta = WHEEL_DELTA;
	}
}