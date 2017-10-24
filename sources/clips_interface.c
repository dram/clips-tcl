#include <assert.h>

#include <tcl.h>

#include "clips/clips.h"

#include "interface.h"

static Tcl_Obj *tcl_clips_ValueToObj(Environment *env, CLIPSValue *value)
{
	switch (value->header->type) {
	case SYMBOL_TYPE:
		if (value->value == TrueSymbol(env)) {
			return Tcl_NewBooleanObj(true);
		} else if (value->value == FalseSymbol(env)) {
			return Tcl_NewBooleanObj(false);
		} else {
			return Tcl_NewStringObj(
				value->lexemeValue->contents, -1);
		}
	case INTEGER_TYPE:
		return Tcl_NewIntObj(value->integerValue->contents);
	case STRING_TYPE:
		return Tcl_NewStringObj(value->lexemeValue->contents, -1);
	default:
		assert(false);
		return NULL;
	}
}

static int tcl_CLIPS_AssertString(Tcl_Interp *interp,
				  Environment *env,
				  int objc,
				  Tcl_Obj *const objv[])
{
	AssertString(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_CLIPS_BatchStar(Tcl_Interp *interp,
			       Environment *env,
			       int objc,
			       Tcl_Obj *const objv[])
{
	BatchStar(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_CLIPS_Build(Tcl_Interp *interp,
			   Environment *env,
			   int objc,
			   Tcl_Obj *const objv[])
{
	Build(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_CLIPS_DefglobalGetValue(Tcl_Interp *interp,
				       Environment *env,
				       int objc,
				       Tcl_Obj *const objv[])
{
	CLIPSValue v;
	DefglobalGetValue(
		*(void **) Tcl_GetByteArrayFromObj(objv[2], NULL), &v);
	Tcl_SetObjResult(interp, tcl_clips_ValueToObj(env, &v));

	return TCL_OK;
}

static int tcl_CLIPS_Eval(Tcl_Interp *interp,
			  Environment *env,
			  int objc,
			  Tcl_Obj *const objv[])
{
	Eval(env, Tcl_GetString(objv[2]), NULL);

	return TCL_OK;
}

static int tcl_CLIPS_FindDefglobal(Tcl_Interp *interp,
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

static int tcl_CLIPS_Load(Tcl_Interp *interp,
			  Environment *env,
			  int objc,
			  Tcl_Obj *const objv[])
{
	Load(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_CLIPS_LoadFacts(Tcl_Interp *interp,
			       Environment *env,
			       int objc,
			       Tcl_Obj *const objv[])
{
	LoadFacts(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_CLIPS_Reset(Tcl_Interp *interp,
			   Environment *env,
			   int objc,
			   Tcl_Obj *const objv[])
{
	Reset(env);

	return TCL_OK;
}

static int tcl_CLIPS_Run(Tcl_Interp *interp,
			 Environment *env,
			 int objc,
			 Tcl_Obj *const objv[])
{
	int limit;
	Tcl_GetIntFromObj(interp, objv[2], &limit);
	Run(env, limit);

	return TCL_OK;
}

static int tcl_CLIPS_SetStrategy(Tcl_Interp *interp,
				 Environment *env,
				 int objc,
				 Tcl_Obj *const objv[])
{
	int st;

	const char *p = Tcl_GetString(objv[2]);

	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		switch (*p) {
		case 'b':
			assert(strncmp(p, "breadth-strategy", 16) == 0);
			st |= BREADTH_STRATEGY;
			p += 16;
			break;
		case 'c':
			assert(strncmp(p, "complexity-strategy", 19) == 0);
			st |= COMPLEXITY_STRATEGY;
			p += 19;
			break;
		case 'd':
			assert(strncmp(p, "depth-strategy", 14) == 0);
			st |= DEPTH_STRATEGY;
			p += 14;
			break;
		case 'l':
			assert(strncmp(p, "lex-strategy", 12) == 0);
			st |= LEX_STRATEGY;
			p += 12;
			break;
		case 'm':
			assert(strncmp(p, "mea-strategy", 12) == 0);
			st |= MEA_STRATEGY;
			p += 12;
			break;
		case 'r':
			assert(strncmp(p, "random-strategy", 15) == 0);
			st |= RANDOM_STRATEGY;
			p += 15;
			break;
		case 's':
			assert(strncmp(p, "simplicity-strategy", 19) == 0);
			st |= SIMPLICITY_STRATEGY;
			p += 19;
			break;
		default:
			assert(false);
		}
	}

	SetStrategy(env, st);

	// TODO: Return old setting

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
		return tcl_CLIPS_AssertString(interp, env, objc, objv);
	case 'b':
		switch (command[1]) {
		case 'a':
			assert(strcmp(command, "batch-star") == 0);
			assert(objc == 3);
			return tcl_CLIPS_BatchStar(interp, env, objc, objv);
		case 'u':
			assert(strcmp(command, "build") == 0);
			assert(objc == 3);
			return tcl_CLIPS_Build(interp, env, objc, objv);
		default:
			assert(false);
		}
		break;
	case 'd':
		assert(strcmp(command, "defglobal-get-value") == 0);
		assert(objc == 3);
		return tcl_CLIPS_DefglobalGetValue(interp, env, objc, objv);
	case 'e':
		assert(strcmp(command, "eval") == 0);
		assert(objc == 3);
		return tcl_CLIPS_Eval(interp, env, objc, objv);
	case 'f':
		assert(strcmp(command, "find-defglobal") == 0);
		assert(objc == 3);
		return tcl_CLIPS_FindDefglobal(interp, env, objc, objv);
	case 'l':
		switch (command[4]) {
		case 0:
			assert(strcmp(command, "load") == 0);
			assert(objc == 3);
			return tcl_CLIPS_Load(interp, env, objc, objv);
		case '-':
			assert(strcmp(command, "load-facts") == 0);
			assert(objc == 3);
			return tcl_CLIPS_LoadFacts(interp, env, objc, objv);
		default:
			assert(false);
		}
		break;
	case 'r':
		switch (command[1]) {
		case 'e':
			assert(strcmp(command, "reset") == 0);
			assert(objc == 2);
			return tcl_CLIPS_Reset(interp, env, objc, objv);
		case 'u':
			assert(strcmp(command, "run") == 0);
			assert(objc == 3);
			return tcl_CLIPS_Run(interp, env, objc, objv);
		default:
			assert(false);
		}
		break;
	case 's':
		assert(strcmp(command, "set-strategy") == 0);
		assert(objc == 3);
		return tcl_CLIPS_SetStrategy(interp, env, objc, objv);
	default:
		assert(false);
	}

	return TCL_ERROR;
}

void CLIPS_Tcl_InitializeCLIPSInterface(Environment *env, Tcl_Interp *interp)
{
	Tcl_CreateObjCommand(interp, "clips", clips_Tcl_ObjCmdProc, env, NULL);
}
