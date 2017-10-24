#include <assert.h>

#include <tcl.h>

#include "clips/clips.h"

#include "interface.h"


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

	char *command = Tcl_GetString(objv[1]);

	switch (command[0]) {
	case 'a':
		assert(strcmp(command, "assert-string") == 0);
		assert(objc == 3);
		AssertString(env, Tcl_GetString(objv[2]));
		break;
	case 'b':
		switch (command[1]) {
		case 'a':
			assert(strcmp(command, "batch-star") == 0);
			assert(objc == 3);
			BatchStar(env, Tcl_GetString(objv[2]));
			break;
		case 'u':
			assert(strcmp(command, "build") == 0);
			assert(objc == 3);
			Build(env, Tcl_GetString(objv[2]));
			break;
		default:
			assert(false);
		}
		break;
	case 'd':
		assert(strcmp(command, "defglobal-get-value") == 0);
		assert(objc == 3);

		CLIPSValue v;
		DefglobalGetValue(
			*(void **) Tcl_GetByteArrayFromObj(objv[2], NULL),
			&v);
		printf("%lld\n", v.integerValue->contents);
		Tcl_SetObjResult(interp,
				 Tcl_NewIntObj(
					 v.integerValue->contents));
		break;
	case 'e':
		assert(strcmp(command, "eval") == 0);
		assert(objc == 3);
		Eval(env, Tcl_GetString(objv[2]), NULL);
		break;
	case 'f':
		assert(strcmp(command, "find-defglobal") == 0);
		assert(objc == 3);
		Defglobal *r = FindDefglobal(env, Tcl_GetString(objv[2]));
		printf("##2 %p\n", r);
		if (r == NULL) {
		} else {
			Tcl_SetObjResult(interp,
					 Tcl_NewByteArrayObj(
						 (void *) &r, sizeof(r)));
		}
		break;
	case 'l':
		switch (command[4]) {
		case 0:
			assert(strcmp(command, "load") == 0);
			assert(objc == 3);
			Load(env, Tcl_GetString(objv[2]));
			break;
		case '-':
			assert(strcmp(command, "load-facts") == 0);
			assert(objc == 3);
			LoadFacts(env, Tcl_GetString(objv[2]));
			break;
		default:
			assert(false);
		}
		break;
	case 'r':
		switch (command[1]) {
		case 'e':
			assert(strcmp(command, "reset") == 0);
			assert(objc == 2);
			Reset(env);
			break;
		case 'u':
			assert(strcmp(command, "run") == 0);
			assert(objc == 3);
			int i;
			Tcl_GetIntFromObj(interp, objv[2], &i);
			Run(env, i);
			break;
		default:
			assert(false);
		}
		break;
	default:
		assert(false);
	}

	return TCL_OK;
}

void CLIPS_Tcl_InitializeCLIPSInterface(Environment *env, Tcl_Interp *interp)
{
	Tcl_CreateObjCommand(interp, "clips", clips_Tcl_ObjCmdProc, env, NULL);
}
