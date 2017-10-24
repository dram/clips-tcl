#include <tcl.h>
#include "clips/clips.h"

#include "interface.h"

void UserFunctions(Environment *env)
{
	// Empty, all work done in AppInit.
}

static void ExitProc(ClientData clientData)
{
	Environment *env = clientData;

	DestroyEnvironment(env);
}

static int AppInit(Tcl_Interp *interp)
{
	Environment *env = CreateEnvironment();

	Tcl_CreateExitHandler(ExitProc, env);

	CLIPS_Tcl_InitializeInterface(env, interp);

	return TCL_OK;
}

int main(int argc, char *argv[])
{
	Tcl_Main(argc, argv, AppInit);

	return 0;
}
