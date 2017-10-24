#include <assert.h>

#include <tcl.h>

#include "clips/clips.h"

#include "interface.h"

void CLIPS_Tcl_InitializeInterface(Environment *env, Tcl_Interp *interp)
{
	CLIPS_Tcl_InitializeCLIPSInterface(env, interp);
	CLIPS_Tcl_InitializeTclInterface(env, interp);
}
