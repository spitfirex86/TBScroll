#include "scroll.h"
#include "worker.h"
#include "main.h"
#include "framework.h"

#define ABS(x) (((x) >= 0) ? (x) : -(x))

HHOOK g_hHook = NULL;

int g_lSensitivityX = 120;
int g_lSensitivityY = 20;

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

	if ( nCode < 0 )
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);

	switch ( wParam )
	{
	case WM_MBUTTONDOWN:
		{
			// Pass emulated click to other hooks
			if ( g_bEmulatingClick )
				break;

			g_bButtonPressed = TRUE;
			g_bScrolling = FALSE;
			g_lStartX = p_stMouse->pt.x;
			g_lStartY = p_stMouse->pt.y;
			g_lMoveX = 0;
			g_lMoveY = 0;

			return TRUE;
		}

	case WM_MBUTTONUP:
		{
			if ( g_bEmulatingClick )
			{
				// We are done emulating the click, pass to other hooks
				g_bEmulatingClick = FALSE;
				break;
			}

			g_bButtonPressed = FALSE;

			// If scrolling did not occur, emulate click
			if ( !g_bScrolling )
			{
				g_bEmulatingClick = TRUE;
				fn_vQueueClick(g_lStartX, g_lStartY);
			}

			return TRUE;
		}

	case WM_MOUSEMOVE:
		{
			if ( !g_bButtonPressed )
				break;

			g_lMoveX += p_stMouse->pt.x - g_lStartX;
			g_lMoveY += p_stMouse->pt.y - g_lStartY;

			// Vertical scroll
			if ( ABS(g_lMoveY) > g_lSensitivityY )
			{
				g_bScrolling = TRUE;

				int lSteps = g_lMoveY / g_lSensitivityY;
				int lDelta = -lSteps * WHEEL_DELTA;

				g_lMoveY -= lSteps * g_lSensitivityY;

				fn_vQueueVScroll(lDelta);
			}

			// Horizontal scroll
			if ( ABS(g_lMoveX) > g_lSensitivityX )
			{
				g_bScrolling = TRUE;

				int lSteps = g_lMoveX / g_lSensitivityX;
				int lDelta = lSteps * WHEEL_DELTA;

				g_lMoveX -= lSteps * g_lSensitivityX;
				g_lMoveY = 0;

				fn_vQueueHScroll(lDelta);
			}

			return TRUE;
		}
	}

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

BOOL fn_bSetSensitivityYFromStr( char const *szStr )
{
	int lBuffer = strtol(szStr, NULL, 10);

	if ( lBuffer <= 0 )
		return FALSE;

	g_lSensitivityY = lBuffer;
	return TRUE;
}

BOOL fn_bSetSensitivityXFromStr( char const *szStr )
{
	int lBuffer = strtol(szStr, NULL, 10);

	if ( lBuffer <= 0 )
		return FALSE;

	g_lSensitivityX = lBuffer;
	return TRUE;
}

void fn_vSaveConfig( void )
{
	char szBuffer[64];

	sprintf(szBuffer, "%d", g_lSensitivityY);
	WritePrivateProfileString(g_szAppName, "VSensitivity", szBuffer, g_szConfigPath);

	sprintf(szBuffer, "%d", g_lSensitivityX);
	WritePrivateProfileString(g_szAppName, "HSensitivity", szBuffer, g_szConfigPath);
}

void fn_vLoadConfig( void )
{
	char szBuffer[64];

	GetPrivateProfileString(g_szAppName, "VSensitivity", "20", szBuffer, sizeof(szBuffer), g_szConfigPath);
	fn_bSetSensitivityYFromStr(szBuffer);

	GetPrivateProfileString(g_szAppName, "HSensitivity", "120", szBuffer, sizeof(szBuffer), g_szConfigPath);
	fn_bSetSensitivityXFromStr(szBuffer);
}