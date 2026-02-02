#include "buttons.h"
#include "framework.h"


tdstScrollButton const g_a_stScrollButtons[e_NbScrollButton] = {
	{ e_SB_MiddleButton, WM_MBUTTONDOWN, WM_MBUTTONUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, 0, "Middle Button" },
	{ e_SB_Button4, WM_XBUTTONDOWN, WM_XBUTTONUP, MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP, XBUTTON1, "Button 4 (XButton1)" },
	{ e_SB_Button5, WM_XBUTTONDOWN, WM_XBUTTONUP, MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP, XBUTTON2, "Button 5 (XButton2)" },
	{ e_SB_Button4M, WM_XBUTTONDOWN, WM_XBUTTONUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, XBUTTON1, "Button 4 (Send MMB)" },
	{ e_SB_Button5M, WM_XBUTTONDOWN, WM_XBUTTONUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, XBUTTON2, "Button 5 (Send MMB)" },
	{ e_SB_RightButton, WM_RBUTTONDOWN, WM_RBUTTONUP, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, 0, "Right Button" },
};

tdeScrollButtonId const g_a_eEmulatedButtons[e_NbScrollButton] = {
	e_SB_MiddleButton,
	e_SB_Button4,
	e_SB_Button5,
	e_SB_MiddleButton,
	e_SB_MiddleButton,
	e_SB_RightButton
};

tdeScrollButtonId g_eCurrentButton = e_SB_MiddleButton;
