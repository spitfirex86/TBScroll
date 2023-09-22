#pragma once
#include "framework.h"


#define C_MaxButtonName 30
#define C_DefaultScrollButton (e_SB_MiddleButton)


typedef enum tdeScrollButtonId_
{
	e_SB_MiddleButton,
	e_SB_Button4,
	e_SB_Button5,
	e_SB_Button4M,
	e_SB_Button5M,

	e_NbScrollButton
}
tdeScrollButtonId;

typedef struct tdstScrollButton_
{
	tdeScrollButtonId eId;
	DWORD ulMessageDown;
	DWORD ulMessageUp;
	DWORD ulEventDown;
	DWORD ulEventUp;
	WORD uwParam;
	char szName[C_MaxButtonName];
}
tdstScrollButton;


extern tdstScrollButton const g_a_stScrollButtons[e_NbScrollButton];
extern tdeScrollButtonId const g_a_eEmulatedButtons[e_NbScrollButton];
extern tdeScrollButtonId g_eCurrentButton;

#define M_p_stGetCurrentButton() (&g_a_stScrollButtons[g_eCurrentButton])
#define M_p_stGetCurrentEmulatedButton() (&g_a_stScrollButtons[g_a_eEmulatedButtons[g_eCurrentButton]])
