#include "main.h"
#include "scroll.h"
#include "utils.h"
#include "framework.h"
#include "resource.h"


#define WM_USER_TRAY (WM_USER+1)

char const C_szAppName[] = "TBScroll";
char const C_szWindowClass[] = "TBSCROLL";
char const C_szConfigPath[] = ".\\tbscroll.ini";

HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;
HICON g_hAppIcon = NULL;

HWND g_hSettingsDlg = NULL;


BOOL fn_bCreateTrayIcon( HWND hWnd )
{
	NOTIFYICONDATA nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_TBSCROLL;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_USER_TRAY;
	nid.hIcon = g_hAppIcon;
	strcpy(nid.szTip, C_szAppName);

	return Shell_NotifyIcon(NIM_ADD, &nid);
}

BOOL fn_vDeleteTrayIcon( HWND hWnd )
{
	NOTIFYICONDATA nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_TBSCROLL;

	return Shell_NotifyIcon(NIM_DELETE, &nid);
}


INT_PTR CALLBACK fn_bAboutDlgProc( HWND hDlg, UINT ulMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( ulMsg )
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL )
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR CALLBACK fn_bSettingsDlgProc( HWND hDlg, UINT ulMsg, WPARAM wParam, LPARAM lParam )
{
	static HWND hSensitivityY = NULL;
	static HWND hSensitivityX = NULL;
	static HWND hReverse = NULL;

	char szBuffer[16];
	int lTmpX, lTmpY;

	switch ( ulMsg )
	{
	case WM_INITDIALOG:
		g_hSettingsDlg = hDlg;

		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)g_hAppIcon);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)g_hAppIcon);

		hSensitivityY = GetDlgItem(hDlg, IDC_VSENSITIVITY);
		SendMessage(hSensitivityY, EM_SETLIMITTEXT, sizeof(szBuffer) - 1, 0);
		sprintf(szBuffer, "%d", g_lSensitivityY);
		Edit_SetText(hSensitivityY, szBuffer);

		hSensitivityX = GetDlgItem(hDlg, IDC_HSENSITIVITY);
		SendMessage(hSensitivityX, EM_SETLIMITTEXT, sizeof(szBuffer) - 1, 0);
		sprintf(szBuffer, "%d", g_lSensitivityX);
		Edit_SetText(hSensitivityX, szBuffer);

		hReverse = GetDlgItem(hDlg, IDC_REVERSE);
		Button_SetCheck(hReverse, g_bReverse);

		break;

	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDOK:
				Edit_GetText(hSensitivityY, szBuffer, sizeof(szBuffer));
				if ( !fn_bIntFromStr(szBuffer, &lTmpY) )
				{
					SetFocus(hSensitivityY);
					break;
				}

				Edit_GetText(hSensitivityX, szBuffer, sizeof(szBuffer));
				if ( !fn_bIntFromStr(szBuffer, &lTmpX) )
				{
					SetFocus(hSensitivityX);
					break;
				}

				g_lSensitivityY = lTmpY;
				g_lSensitivityX = lTmpX;

				g_bReverse = Button_GetCheck(hReverse);

				fn_vSaveConfig();
				// fall through
			case IDCANCEL:
				EndDialog(hDlg, LOWORD(wParam));
				break;

			default:
				return FALSE;
			}
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, IDCANCEL);
		break;

	case WM_DESTROY:
		g_hSettingsDlg = NULL;
		hSensitivityY = NULL;
		hSensitivityX = NULL;
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL fn_bProcessTrayMsg( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	switch ( lParam )
	{
	case WM_RBUTTONUP:
		{
			HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_TBSCROLL));
			HMENU hTrayMenu = GetSubMenu(hMenu, 0);
			POINT stClick = { 0 };
			GetCursorPos(&stClick);

			// Hack to correctly handle focus on the popup menu.
			// As described in the "remarks" section of the TrackPopupMenu docs.
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hTrayMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN, stClick.x, stClick.y, 0, hWnd, NULL);
			PostMessage(hWnd, WM_NULL, 0, 0);
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL fn_bProcessCmds( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	switch ( LOWORD(wParam) )
	{
	case IDM_SETTINGS:
		g_hSettingsDlg
			? SetForegroundWindow(g_hSettingsDlg)
			: DialogBox(g_hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, fn_bSettingsDlgProc);
		break;

	case IDM_ABOUT:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, fn_bAboutDlgProc);
		break;

	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK fn_lMainWndProc( HWND hWnd, UINT ulMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( ulMsg )
	{
	case WM_CREATE:
		fn_bCreateTrayIcon(hWnd);
		break;

	case WM_USER_TRAY:
		if ( !fn_bProcessTrayMsg(hWnd, wParam, lParam) )
			DefWindowProc(hWnd, ulMsg, wParam, lParam);
		break;

	case WM_COMMAND:
		if ( !fn_bProcessCmds(hWnd, wParam, lParam) )
			DefWindowProc(hWnd, ulMsg, wParam, lParam);
		break;

	case WM_DESTROY:
		fn_vDeleteTrayIcon(hWnd);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, ulMsg, wParam, lParam);
	}

	return 0;
}

ATOM fn_hRegisterWndClass( HINSTANCE hInst )
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = fn_lMainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = g_hAppIcon;
	wcex.hIconSm = g_hAppIcon;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = C_szWindowClass;

	return RegisterClassEx(&wcex);
}

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPCSTR lpCmdLine, int nCmdShow )
{
	MSG msg;

	HANDLE hMutex = CreateMutex(NULL, TRUE, C_szAppName);
	if ( !hMutex || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		MessageBox(NULL, "Another instance of TBScroll is already running.", C_szAppName, MB_OK | MB_ICONERROR);
		return 0;
	}

	g_hAppIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TBSCROLL));
	fn_hRegisterWndClass(hInst);
	HWND hWnd = CreateWindow(C_szWindowClass, C_szAppName, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInst, NULL);

	if ( !hWnd )
		return 0;

	g_hInst = hInst;
	g_hWnd = hWnd;

	//ShowWindow(hWnd, nCmdShow);
	SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE);
	UpdateWindow(hWnd);

	fn_vLoadConfig();
	if ( !fn_bInitScroll() )
	{
		MessageBox(NULL, "Cannot initialize TBScroll.", C_szAppName, MB_OK | MB_ICONERROR);
		return 0;
	}

	while ( GetMessage(&msg, NULL, 0, 0) > 0 )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	fn_vDeInitScroll();

	ReleaseMutex(hMutex);
	return (int)msg.wParam;
}
