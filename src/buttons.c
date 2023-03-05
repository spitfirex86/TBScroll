#include "buttons.h"
#include "framework.h"


tdstScrollButton const g_a_stScrollButtons[e_NbScrollButton] = {
	{ e_SB_MiddleButton, WM_MBUTTONDOWN, WM_MBUTTONUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, 0, "Middle Button" },
	{ e_SB_Button4, WM_XBUTTONDOWN, WM_XBUTTONUP, MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP, XBUTTON1, "Button 4 (XButton1)" },
	{ e_SB_Button5, WM_XBUTTONDOWN, WM_XBUTTONUP, MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP, XBUTTON2, "Button 5 (XButton2)" },
};

tdeScrollButtonId g_eCurrentButton = e_SB_MiddleButton;
