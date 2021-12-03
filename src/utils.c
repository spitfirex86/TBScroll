#include "utils.h"
#include "main.h"
#include "framework.h"


BOOL fn_bIntFromStr( char const *szIn, int *p_lOut )
{
	char *pLast;

	errno = 0;
	int lValue = strtol(szIn, &pLast, 10);

	if ( errno == ERANGE || *pLast != 0 || pLast == szIn )
		return FALSE;

	*p_lOut = lValue;
	return TRUE;
}

int fn_lReadIntFromCfg( char const *szKey, int lDefault )
{
	char szBuffer[64];
	int lValue = 0;

	GetPrivateProfileString(C_szAppName, szKey, NULL, szBuffer, sizeof(szBuffer), C_szConfigPath);

	if ( !fn_bIntFromStr(szBuffer, &lValue) )
		return lDefault;

	return lValue;
}

void fn_vWriteIntToCfg( char const *szKey, int lValue )
{
	char szBuffer[64];

	sprintf(szBuffer, "%d", lValue);
	WritePrivateProfileString(C_szAppName, szKey, szBuffer, C_szConfigPath);
}