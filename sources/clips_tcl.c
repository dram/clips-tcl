#include <tcl.h>
#include "clips/clips.h"

#include "interface.h"

static Tcl_Interp *Default_Interp;

static void CleanupFunction(Environment *env)
{
	Tcl_DeleteInterp(Default_Interp);
}

void UserFunctions(Environment *env)
{
	Tcl_Interp *interp = Tcl_CreateInterp();

	Default_Interp = interp;

	// According to manual, priority -2000 to 2000 are reserved by CLIPS.
	AddEnvironmentCleanupFunction(
		env, "CleanupFunction", CleanupFunction, 5000);

	CLIPS_Tcl_Interface(env, interp);
}
