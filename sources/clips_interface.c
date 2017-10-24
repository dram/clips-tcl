#include <assert.h>

#include <tcl.h>

#include "clips/clips.h"

#include "interface.h"

static int tcl_clips_AssertString(Tcl_Interp *interp,
				  Environment *env,
				  int objc,
				  Tcl_Obj *const objv[])
{
	AssertString(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_clips_BatchStar(Tcl_Interp *interp,
			       Environment *env,
			       int objc,
			       Tcl_Obj *const objv[])
{
	BatchStar(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_clips_Build(Tcl_Interp *interp,
			   Environment *env,
			   int objc,
			   Tcl_Obj *const objv[])
{
	Build(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_clips_DefglobalGetValue(Tcl_Interp *interp,
				       Environment *env,
				       int objc,
				       Tcl_Obj *const objv[])
{
	CLIPSValue v;
	DefglobalGetValue(
		*(void **) Tcl_GetByteArrayFromObj(objv[2], NULL), &v);
	Tcl_SetObjResult(interp, Tcl_NewIntObj(v.integerValue->contents));

	return TCL_OK;
}

static int tcl_clips_Eval(Tcl_Interp *interp,
			  Environment *env,
			  int objc,
			  Tcl_Obj *const objv[])
{
	Eval(env, Tcl_GetString(objv[2]), NULL);

	return TCL_OK;
}

static int tcl_clips_FindDefglobal(Tcl_Interp *interp,
				   Environment *env,
				   int objc,
				   Tcl_Obj *const objv[])
{
	Defglobal *r = FindDefglobal(env, Tcl_GetString(objv[2]));

	if (r == NULL) {
	} else {
		Tcl_SetObjResult(interp,
				 Tcl_NewByteArrayObj((void *) &r, sizeof(r)));
	}

	return TCL_OK;
}

static int tcl_clips_Load(Tcl_Interp *interp,
			  Environment *env,
			  int objc,
			  Tcl_Obj *const objv[])
{
	Load(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_clips_LoadFacts(Tcl_Interp *interp,
			       Environment *env,
			       int objc,
			       Tcl_Obj *const objv[])
{
	LoadFacts(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_clips_Reset(Tcl_Interp *interp,
			   Environment *env,
			   int objc,
			   Tcl_Obj *const objv[])
{
	Reset(env);

	return TCL_OK;
}

static int tcl_clips_Run(Tcl_Interp *interp,
			 Environment *env,
			 int objc,
			 Tcl_Obj *const objv[])
{
	int limit;
	Tcl_GetIntFromObj(interp, objv[2], &limit);
	Run(env, limit);

	return TCL_OK;
}

static int clips_Tcl_ObjCmdProc(ClientData clientData,
				Tcl_Interp *interp,
				int objc,
				Tcl_Obj *const objv[])
{
	Environment *env = clientData;

	if (objc < 2) {
		Tcl_WrongNumArgs(
			interp, 0, NULL, "clips command ...");
		return TCL_ERROR;
	}

	const char *command = Tcl_GetString(objv[1]);

	switch (command[0]) {
	case 'a':
		assert(strcmp(command, "assert-string") == 0);
		assert(objc == 3);
		return tcl_clips_AssertString(interp, env, objc, objv);
	case 'b':
		switch (command[1]) {
		case 'a':
			assert(strcmp(command, "batch-star") == 0);
			assert(objc == 3);
			return tcl_clips_BatchStar(interp, env, objc, objv);
		case 'u':
			assert(strcmp(command, "build") == 0);
			assert(objc == 3);
			return tcl_clips_Build(interp, env, objc, objv);
		default:
			assert(false);
		}
		break;
	case 'd':
		assert(strcmp(command, "defglobal-get-value") == 0);
		assert(objc == 3);
		return tcl_clips_DefglobalGetValue(interp, env, objc, objv);
	case 'e':
		assert(strcmp(command, "eval") == 0);
		assert(objc == 3);
		return tcl_clips_Eval(interp, env, objc, objv);
	case 'f':
		assert(strcmp(command, "find-defglobal") == 0);
		assert(objc == 3);
		return tcl_clips_FindDefglobal(interp, env, objc, objv);
	case 'l':
		switch (command[4]) {
		case 0:
			assert(strcmp(command, "load") == 0);
			assert(objc == 3);
			return tcl_clips_Load(interp, env, objc, objv);
		case '-':
			assert(strcmp(command, "load-facts") == 0);
			assert(objc == 3);
			return tcl_clips_LoadFacts(interp, env, objc, objv);
		default:
			assert(false);
		}
		break;
	case 'r':
		switch (command[1]) {
		case 'e':
			assert(strcmp(command, "reset") == 0);
			assert(objc == 2);
			return tcl_clips_Reset(interp, env, objc, objv);
		case 'u':
			assert(strcmp(command, "run") == 0);
			assert(objc == 3);
			return tcl_clips_Run(interp, env, objc, objv);
		default:
			assert(false);
		}
		break;
	default:
		assert(false);
	}

	return TCL_ERROR;
}

void CLIPS_Tcl_InitializeCLIPSInterface(Environment *env, Tcl_Interp *interp)
{
	Tcl_CreateObjCommand(interp, "clips", clips_Tcl_ObjCmdProc, env, NULL);
}
