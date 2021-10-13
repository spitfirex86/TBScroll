#pragma once
#include "framework.h"


void fn_vScrollWorkerThread( void *unused );

void fn_vQueueClick( int const lX, int const lY );
void fn_vQueueVScroll( int const lDelta );
void fn_vQueueHScroll( int const lDelta );

BOOL fn_bStartWorkerThread( void );
BOOL fn_bStopWorkerThread( void );