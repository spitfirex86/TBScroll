#include "worker.h"
#include "buttons.h"
#include "framework.h"


#define WM_APP_STOPTHREAD (WM_APP+1)
#define WM_APP_CLICK (WM_APP+2)
#define WM_APP_VSCROLL (WM_APP+3)
#define WM_APP_HSCROLL (WM_APP+4)

HANDLE g_hEventWorkerStarted = NULL;
HANDLE g_hWorker = NULL;
DWORD g_ulWorkerId = 0;


// The low-level mouse hook has a limited time to process a single message.
// Input emulation takes too long and results in silent removal of the hook,
// so a separate worker thread is required to emulate scroll wheel input.
void fn_vScrollWorkerThread( void *unused )
{
	MSG msg;

	g_ulWorkerId = GetCurrentThreadId();
	SetEvent(g_hEventWorkerStarted);

	while ( GetMessage(&msg, NULL, 0, 0) > 0 )
	{
		switch ( msg.message )
		{
		case WM_APP_CLICK:
		{
			tdstScrollButton const *p_stButton = M_p_stGetCurrentEmulatedButton();
			mouse_event(p_stButton->ulEventDown, msg.wParam, msg.lParam, p_stButton->uwParam, 0);
			mouse_event(p_stButton->ulEventUp, msg.wParam, msg.lParam, p_stButton->uwParam, 0);
			break;
		}

		case WM_APP_VSCROLL:
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, msg.lParam, 0);
			break;

		case WM_APP_HSCROLL:
			mouse_event(MOUSEEVENTF_HWHEEL, 0, 0, msg.lParam, 0);
			break;

		case WM_APP_STOPTHREAD:
			// Stop the message pump and exit thread
			return;
		}
	}
}

void fn_vQueueClick( int const lX, int const lY )
{
	PostThreadMessage(g_ulWorkerId, WM_APP_CLICK, lX, lY);
}

void fn_vQueueVScroll( int const lDelta )
{
	PostThreadMessage(g_ulWorkerId, WM_APP_VSCROLL, 0, lDelta);
}

void fn_vQueueHScroll( int const lDelta )
{
	PostThreadMessage(g_ulWorkerId, WM_APP_HSCROLL, 0, lDelta);
}


BOOL fn_bStartWorkerThread( void )
{
	g_hEventWorkerStarted = CreateEvent(NULL, TRUE, FALSE, "WorkerThreadStarted");
	g_hWorker = (HANDLE)_beginthread(fn_vScrollWorkerThread, 0, 0);

	BOOL bResult = (WaitForSingleObject(g_hEventWorkerStarted, INFINITE) == WAIT_OBJECT_0);
	CloseHandle(g_hEventWorkerStarted);

	return bResult;
}

BOOL fn_bStopWorkerThread( void )
{
	return PostThreadMessage(g_ulWorkerId, WM_APP_STOPTHREAD, 0, 0);
}
