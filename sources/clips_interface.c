#include <assert.h>

#include <tcl.h>

#include "clips/clips.h"

#include "interface.h"

static Tcl_Obj *tcl_clips_ValueToObj(
	Environment *env, Tcl_Interp *interp, CLIPSValue *value)
{
	Tcl_Obj *obj = NULL;

	switch (value->header->type) {
	case EXTERNAL_ADDRESS_TYPE:
		obj = value->externalAddressValue->contents;
		break;
	case INTEGER_TYPE:
		obj = Tcl_NewIntObj(value->integerValue->contents);
		break;
	case MULTIFIELD_TYPE:
		obj = Tcl_NewObj();

		Multifield *m = value->multifieldValue;
		for (int i = 0; i < m->length; ++i) {
			Tcl_ListObjAppendElement(
				interp,
				obj,
				tcl_clips_ValueToObj(
					env, interp, &m->contents[i]));
		}
		break;
	case STRING_TYPE:
		obj = Tcl_NewStringObj(value->lexemeValue->contents, -1);
		break;
	case SYMBOL_TYPE:
		if (value->value == TrueSymbol(env)) {
			obj = Tcl_NewBooleanObj(true);
		} else if (value->value == FalseSymbol(env)) {
			obj = Tcl_NewBooleanObj(false);
		} else {
			obj = Tcl_NewStringObj(
				value->lexemeValue->contents, -1);
		}
		break;
	case VOID_TYPE:
		break;
	default:
		assert(false);
	}

	return obj;
}

static int tcl_CLIPS_AssertString(Tcl_Interp *interp,
				  Environment *env,
				  int objc,
				  Tcl_Obj *const objv[])
{
	Fact *r = AssertString(env, Tcl_GetString(objv[2]));

	if (r == NULL) {
		return TCL_ERROR;
	} else {
		Tcl_SetObjResult(interp,
				 Tcl_NewByteArrayObj((void *) &r, sizeof(r)));
		return TCL_OK;
	}
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
	BuildError r = Build(env, Tcl_GetString(objv[2]));

	if (r == BE_NO_ERROR)
		return TCL_OK;
	else
		return TCL_ERROR;
}

static int tcl_CLIPS_CreateString(Tcl_Interp *interp,
				  Environment *env,
				  int objc,
				  Tcl_Obj *const objv[])
{
	CLIPSLexeme *v = CreateString(env, Tcl_GetString(objv[2]));

	Tcl_SetObjResult(interp, Tcl_NewByteArrayObj((void *) &v, sizeof(v)));

	return TCL_OK;
}

static int tcl_CLIPS_DefglobalGetValue(Tcl_Interp *interp,
				       Environment *env,
				       int objc,
				       Tcl_Obj *const objv[])
{
	CLIPSValue value;

	Defglobal **p = (void *) Tcl_GetByteArrayFromObj(objv[2], NULL);

	DefglobalGetValue(*p, &value);

	Tcl_SetObjResult(interp, tcl_clips_ValueToObj(env, interp, &value));

	return TCL_OK;
}

static int tcl_CLIPS_DefglobalSetString(Tcl_Interp *interp,
					Environment *env,
					int objc,
					Tcl_Obj *const objv[])
{
	Defglobal **d = (void *) Tcl_GetByteArrayFromObj(objv[2], NULL);
	const char *value = Tcl_GetString(objv[3]);

	DefglobalSetString(*d, value);

	return TCL_OK;
}

static int tcl_CLIPS_DefglobalSetValue(Tcl_Interp *interp,
				       Environment *env,
				       int objc,
				       Tcl_Obj *const objv[])
{
	Defglobal **d = (void *) Tcl_GetByteArrayFromObj(objv[2], NULL);
	CLIPSValue **in = (void *) Tcl_GetByteArrayFromObj(objv[3], NULL);

	DefglobalSetValue(*d, *in);

	return TCL_OK;
}

static int tcl_CLIPS_Eval(Tcl_Interp *interp,
			  Environment *env,
			  int objc,
			  Tcl_Obj *const objv[])
{
	CLIPSValue value;

	EvalError r = Eval(env, Tcl_GetString(objv[2]), &value);

	if (r == EE_NO_ERROR) {
		Tcl_Obj *obj = tcl_clips_ValueToObj(env, interp, &value);

		if (obj == NULL)
			Tcl_ResetResult(interp);
		else
			Tcl_SetObjResult(interp, obj);

		return TCL_OK;
	} else {
		return TCL_ERROR;
	}
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
	LoadError r = Load(env, Tcl_GetString(objv[2]));

	if (r == LE_NO_ERROR)
		return TCL_OK;
	else
		return TCL_ERROR;
}

static int tcl_CLIPS_LoadFacts(Tcl_Interp *interp,
			       Environment *env,
			       int objc,
			       Tcl_Obj *const objv[])
{
	LoadFacts(env, Tcl_GetString(objv[2]));

	return TCL_OK;
}

static int tcl_CLIPS_ReleaseLexeme(Tcl_Interp *interp,
				   Environment *env,
				   int objc,
				   Tcl_Obj *const objv[])
{
	CLIPSLexeme **value = (void *) Tcl_GetByteArrayFromObj(objv[2], NULL);

	ReleaseLexeme(env, *value);

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

static int tcl_CLIPS_RetainLexeme(Tcl_Interp *interp,
				  Environment *env,
				  int objc,
				  Tcl_Obj *const objv[])
{
	CLIPSLexeme **value = (void *) Tcl_GetByteArrayFromObj(objv[2], NULL);

	RetainLexeme(env, *value);

	return TCL_OK;
}

static int tcl_CLIPS_Run(Tcl_Interp *interp,
			 Environment *env,
			 int objc,
			 Tcl_Obj *const objv[])
{
	int limit;
	Tcl_GetIntFromObj(interp, objv[2], &limit);

	int r = Run(env, limit);

	Tcl_SetObjResult(interp, Tcl_NewIntObj(r));

	return TCL_OK;
}

static int tcl_CLIPS_SetStrategy(Tcl_Interp *interp,
				 Environment *env,
				 int objc,
				 Tcl_Obj *const objv[])
{
	int st = 0;

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

static int tcl_clips_ParseWatchOptions(const char *p)
{
	int item = 0;

	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		switch (*p) {
		case 'a':
			switch (p[1]) {
			case 'l':
				assert(strncmp(p, "all", 3) == 0);
				item |= ALL;
				p += 3;
				break;
			case 'c':
				assert(strncmp(p, "activations", 11) == 0);
				item |= ACTIVATIONS;
				p += 11;
				break;
			}
			break;
		case 'c':
			assert(strncmp(p, "compilations", 12) == 0);
			item |= COMPILATIONS;
			p += 12;
			break;
		case 'd':
			assert(strncmp(p, "deffunctions", 12) == 0);
			item |= DEFFUNCTIONS;
			p += 12;
			break;
		case 'f':
			switch (p[1]) {
			case 'a':
				assert(strncmp(p, "facts", 5) == 0);
				item |= FACTS;
				p += 5;
				break;
			case 'o':
				assert(strncmp(p, "focus", 5) == 0);
				item |= FOCUS;
				p += 5;
				break;
			}
			break;
		case 'g':
			switch (p[1]) {
			case 'e':
				assert(strncmp(p,
					       "generic-functions", 17) == 0);
				item |= GENERIC_FUNCTIONS;
				p += 17;
				break;
			case 'l':
				assert(strncmp(p, "globals", 7) == 0);
				item |= GLOBALS;
				p += 7;
				break;
			}
			break;
		case 'i':
			assert(strncmp(p, "instances", 9) == 0);
			item |= INSTANCES;
			p += 9;
			break;
		case 'm':
			switch (p[7]) {
			case 0:
				assert(strncmp(p, "methods", 7) == 0);
				item |= METHODS;
				p += 7;
				break;
			case '-':
				assert(strncmp(p,
					       "message-handlers", 16) == 0);
				item |= MESSAGE_HANDLERS;
				p += 16;
				break;
			case 's':
				assert(strncmp(p, "messages", 8) == 0);
				item |= MESSAGES;
				p += 8;
				break;
			}
			break;
		case 's':
			switch (p[1]) {
			case 'l':
				assert(strncmp(p, "slots", 5) == 0);
				item |= SLOTS;
				p += 5;
				break;
			case 't':
				assert(strncmp(p, "statistics", 12) == 0);
				item |= STATISTICS;
				p += 12;
				break;
			}
			break;
		case 'r':
			assert(strncmp(p, "rules", 5) == 0);
			item |= RULES;
			p += 5;
			break;
		default:
			assert(false);
		}
	}

	return item;
}

static int tcl_CLIPS_Unwatch(Tcl_Interp *interp,
			     Environment *env,
			     int objc,
			     Tcl_Obj *const objv[])
{
	const char *p = Tcl_GetString(objv[2]);

	Unwatch(env, tcl_clips_ParseWatchOptions(p));

	return TCL_OK;
}

static int tcl_CLIPS_Watch(Tcl_Interp *interp,
			   Environment *env,
			   int objc,
			   Tcl_Obj *const objv[])
{
	const char *p = Tcl_GetString(objv[2]);

	Watch(env, tcl_clips_ParseWatchOptions(p));

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
	case 'c':
		assert(strcmp(command, "create-string") == 0);
		assert(objc == 3);
		return tcl_CLIPS_CreateString(interp, env, objc, objv);
	case 'd':
		switch (command[10]) {
		case 'g':
			assert(strcmp(command, "defglobal-get-value") == 0);
			assert(objc == 3);
			return tcl_CLIPS_DefglobalGetValue(interp,
							   env, objc, objv);
		case 's':
			switch (command[14]) {
			case 's':
				assert(strcmp(command,
					      "defglobal-set-string") == 0);
				assert(objc == 4);
				return tcl_CLIPS_DefglobalSetString(
					interp, env, objc, objv);
			case 'v':
				assert(strcmp(command,
					      "defglobal-set-value") == 0);
				assert(objc == 4);
				return tcl_CLIPS_DefglobalSetValue(
					interp, env, objc, objv);
			default:
				assert(false);
			}

		default:
			assert(false);
		}
		break;
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
		switch (command[2]) {
		case 'l':
			assert(strcmp(command, "release-lexeme") == 0);
			assert(objc == 3);
			return tcl_CLIPS_ReleaseLexeme(interp,
						       env, objc, objv);
		case 'n':
			assert(strcmp(command, "run") == 0);
			assert(objc == 3);
			return tcl_CLIPS_Run(interp, env, objc, objv);
		case 's':
			assert(strcmp(command, "reset") == 0);
			assert(objc == 2);
			return tcl_CLIPS_Reset(interp, env, objc, objv);
		case 't':
			assert(strcmp(command, "retain-lexeme") == 0);
			assert(objc == 3);
			return tcl_CLIPS_RetainLexeme(interp, env, objc, objv);
		default:
			assert(false);
		}
		break;
	case 's':
		assert(strcmp(command, "set-strategy") == 0);
		assert(objc == 3);
		return tcl_CLIPS_SetStrategy(interp, env, objc, objv);
	case 'u':
		assert(strcmp(command, "unwatch") == 0);
		assert(objc == 3);
		return tcl_CLIPS_Unwatch(interp, env, objc, objv);
	case 'w':
		assert(strcmp(command, "watch") == 0);
		assert(objc == 3);
		return tcl_CLIPS_Watch(interp, env, objc, objv);
	default:
		assert(false);
	}

	return TCL_ERROR;
}

void CLIPS_Tcl_InitializeCLIPSInterface(Environment *env, Tcl_Interp *interp)
{
	Tcl_CreateObjCommand(interp, "clips", clips_Tcl_ObjCmdProc, env, NULL);
}
