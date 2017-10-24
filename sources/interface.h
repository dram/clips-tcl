#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <tcl.h>
#include "clips/clips.h"

void CLIPS_Tcl_InitializeInterface(Environment *env, Tcl_Interp *interp);
void CLIPS_Tcl_InitializeCLIPSInterface(Environment *env, Tcl_Interp *interp);
void CLIPS_Tcl_InitializeTclInterface(Environment *env, Tcl_Interp *interp);

#endif // _INTERFACE_H
