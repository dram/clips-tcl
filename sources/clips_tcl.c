#include <assert.h>

#include <tcl.h>

#include "clips/clips.h"

/// Notes:
///
/// 1. Multifield is used when API require an array and count
///    (e.g. Tcl_EvalObjv).
/// 2. Several API use integer to represent flags (e.g. Tcl_EvalObjv,
///    Tcl_OpenCommandChannel), but CLIPS currently do not support
///    bitwise operations, and also it seems that there is no easy to
///    define CLIPS constants in C side, so here we use a formatted
///    symbol (delimited by `/`).
/// 3. As there is no way to count octets in CLIPS, so we count it in
///    C side (e.g. Tcl_NewStringObj).
/// 4. Several API in Tcl will modify passed pointer argument
///    (e.g. Tcl_SplitList), as a mechanism to return multiple values,
///    which CLIPS can not simulate. Here we try to override return
///    value for different types and using something like `/ok/` to
///    represent function call status.

#define FLAG_BUF_LEN 23 // max int64 string length (20) with two '/' delimiter

#define OK_FLAG_DATA_ID (USER_ENVIRONMENT_DATA + 0)
#define ERROR_FLAG_DATA_ID (USER_ENVIRONMENT_DATA + 1)
#define ZERO_FLAG_DATA_ID (USER_ENVIRONMENT_DATA + 2)
#define MINUS_ONE_FLAG_DATA_ID (USER_ENVIRONMENT_DATA + 3)

#define OkFlag(env) \
	(*((CLIPSLexeme **) GetEnvironmentData(env, OK_FLAG_DATA_ID)))
#define ErrorFlag(env) \
	(*((CLIPSLexeme **) GetEnvironmentData(env, ERROR_FLAG_DATA_ID)))
#define ZeroFlag(env) \
	(*((CLIPSLexeme **) GetEnvironmentData(env, ZERO_FLAG_DATA_ID)))
#define MinusOneFlag(env) \
	(*((CLIPSLexeme **) GetEnvironmentData(env, MINUS_ONE_FLAG_DATA_ID)))

enum {
	CLIPS_TCL_CHANNEL_EXTERNAL_ADDRESS = C_POINTER_EXTERNAL_ADDRESS + 1,
	CLIPS_TCL_INTERP_EXTERNAL_ADDRESS,
	CLIPS_TCL_OBJ_EXTERNAL_ADDRESS,
	CLIPS_TCL_STAT_BUF_EXTERNAL_ADDRESS
};

static void clips_Tcl_AllocStatBuf(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_AllocStatBuf(),
		CLIPS_TCL_STAT_BUF_EXTERNAL_ADDRESS);
}

static void clips_Tcl_Close(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue channel;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &channel);

	out->integerValue = CreateInteger(
		env,
		Tcl_Close(interp.externalAddressValue->contents,
			  channel.externalAddressValue->contents));
}

static void clips_Tcl_Concat(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue argv;

	UDFNthArgument(udfc, 1, MULTIFIELD_BIT, &argv);

	int argc = argv.multifieldValue->length;

	size_t argvContentsSize = argc * sizeof (const char *);
	const char **argvContents = genalloc(env, argvContentsSize);
	CLIPSValue *fields = argv.multifieldValue->contents;
	for (int i = 0; i < argc; ++i) {
		assert(fields[i].header->type == STRING_TYPE);
		argvContents[i] = fields[i].lexemeValue->contents;
	}

	char *r = Tcl_Concat(argc, argvContents);
	out->lexemeValue = CreateString(env, r);
	Tcl_Free(r);

	genfree(env, argvContents, argvContentsSize);
}

static void clips_Tcl_CreateInterp(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_CreateInterp(),
		CLIPS_TCL_INTERP_EXTERNAL_ADDRESS);
}

static void clips_Tcl_DecrRefCount(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue objPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &objPtr);

	Tcl_DecrRefCount((Tcl_Obj *) objPtr.externalAddressValue->contents);
}

static void clips_Tcl_DeleteInterp(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);

	Tcl_DeleteInterp(interp.externalAddressValue->contents);
}

static void clips_Tcl_DoOneEvent(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue flags;

	UDFNthArgument(udfc, 1, SYMBOL_BIT, &flags);

	int flagsContents = 0;
	const char *p = flags.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		switch (*p) {
		case 'a':
			assert(strncmp(p, "all-events", 10) == 0);
			flagsContents |= TCL_ALL_EVENTS;
			p += 10;
			break;
		case 'd':
			assert(strncmp(p, "dont-wait", 9) == 0);
			flagsContents |= TCL_DONT_WAIT;
			p += 9;
			break;
		case 'f':
			assert(strncmp(p, "file-events", 11) == 0);
			flagsContents |= TCL_FILE_EVENTS;
			p += 11;
			break;
		case 'i':
			assert(strncmp(p, "idle-events", 11) == 0);
			flagsContents |= TCL_IDLE_EVENTS;
			p += 11;
			break;
		case 't':
			assert(strncmp(p, "timer-events", 12) == 0);
			flagsContents |= TCL_TIMER_EVENTS;
			p += 12;
			break;
		case 'w':
			assert(strncmp(p, "window-events", 13) == 0);
			flagsContents |= TCL_WINDOW_EVENTS;
			p += 13;
			break;
		default:
			assert(false);
		}
	}

	out->integerValue = CreateInteger(env, Tcl_DoOneEvent(flagsContents));
}

static void clips_Tcl_EvalEx(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue script;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, STRING_BIT, &script);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int numBytes = strlen(script.lexemeValue->contents);

	int flagsContents = 0;
	const char *p = flags.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		assert(strncmp(p, "eval-", 5) == 0);
		p += 5;
		switch (*p) {
		case 'd':
			assert(strncmp(p, "direct", 6) == 0);
			flagsContents |= TCL_EVAL_DIRECT;
			p += 6;
			break;
		case 'g':
			assert(strncmp(p, "global", 6) == 0);
			flagsContents |= TCL_EVAL_GLOBAL;
			p += 6;
			break;
		default:
			assert(false);
		}
	}

	int r = Tcl_EvalEx(interp.externalAddressValue->contents,
			   script.externalAddressValue->contents,
			   numBytes,
			   flagsContents);

	switch (r) {
		char buf[FLAG_BUF_LEN];

	case TCL_OK:
		out->lexemeValue = OkFlag(env);
		break;
	case TCL_ERROR:
		out->lexemeValue = ErrorFlag(env);
		break;
	default:
		snprintf(buf, FLAG_BUF_LEN, "/%d/", r);
		out->lexemeValue = CreateSymbol(env, buf);
	}
}

static void clips_Tcl_EvalObjEx(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue objPtr;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &objPtr);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int flagsContents = 0;
	const char *p = flags.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		assert(strncmp(p, "eval-", 5) == 0);
		p += 5;
		switch (*p) {
		case 'd':
			assert(strncmp(p, "direct", 6) == 0);
			flagsContents |= TCL_EVAL_DIRECT;
			p += 6;
			break;
		case 'g':
			assert(strncmp(p, "global", 6) == 0);
			flagsContents |= TCL_EVAL_GLOBAL;
			p += 6;
			break;
		default:
			assert(false);
		}
	}

	int r = Tcl_EvalObjEx(interp.externalAddressValue->contents,
			      objPtr.externalAddressValue->contents,
			      flagsContents);

	switch (r) {
		char buf[FLAG_BUF_LEN];

	case TCL_OK:
		out->lexemeValue = OkFlag(env);
		break;
	case TCL_ERROR:
		out->lexemeValue = ErrorFlag(env);
		break;
	default:
		snprintf(buf, FLAG_BUF_LEN, "/%d/", r);
		out->lexemeValue = CreateSymbol(env, buf);
	}
}

static void clips_Tcl_EvalObjv(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue objv;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, MULTIFIELD_BIT, &objv);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int objc = objv.multifieldValue->length;

	size_t objvContentsSize = objc * sizeof (Tcl_Obj *);
	Tcl_Obj **objvContents = genalloc(env, objvContentsSize);
	CLIPSValue *fields = objv.multifieldValue->contents;
	for (int i = 0; i < objc; ++i) {
		assert(fields[i].header->type == EXTERNAL_ADDRESS_TYPE);
		objvContents[i] = fields[i].externalAddressValue->contents;
	}

	int flagsContents = 0;
	const char *p = flags.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		assert(strncmp(p, "eval-", 5) == 0);
		p += 5;
		switch (*p) {
		case 'd':
			assert(strncmp(p, "direct", 6) == 0);
			flagsContents |= TCL_EVAL_DIRECT;
			p += 6;
			break;
		case 'g':
			assert(strncmp(p, "global", 6) == 0);
			flagsContents |= TCL_EVAL_GLOBAL;
			p += 6;
			break;
		default:
			assert(false);
		}
	}

	int r = Tcl_EvalObjv(interp.externalAddressValue->contents,
			     objc,
			     objvContents,
			     flagsContents);

	switch (r) {
		char buf[FLAG_BUF_LEN];

	case TCL_OK:
		out->lexemeValue = OkFlag(env);
		break;
	case TCL_ERROR:
		out->lexemeValue = ErrorFlag(env);
		break;
	default:
		snprintf(buf, FLAG_BUF_LEN, "/%d/", r);
		out->lexemeValue = CreateSymbol(env, buf);
	}

	genfree(env, objvContents, objvContentsSize);
}

static void clips_Tcl_Flush(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue channel;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &channel);

	int r = Tcl_Flush(channel.externalAddressValue->contents);

	switch (r) {
	case TCL_OK:
		out->lexemeValue = OkFlag(env);
		break;
	case TCL_ERROR:
		out->lexemeValue = ErrorFlag(env);
		break;
	default:
		assert(false);
	}
}

static void clips_Tcl_FSCreateDirectory(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue pathPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &pathPtr);

	int r = Tcl_FSCreateDirectory(pathPtr.externalAddressValue->contents);

	switch (r) {
	case TCL_OK:
		out->lexemeValue = OkFlag(env);
		break;
	case TCL_ERROR:
		out->lexemeValue = ErrorFlag(env);
		break;
	default:
		assert(false);
	}
}

static void clips_Tcl_FSRemoveDirectory(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue pathPtr;
	UDFValue recursive;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &pathPtr);
	UDFNthArgument(udfc, 2, BOOLEAN_BIT, &recursive);

	bool recursiveContents = (recursive.value == TrueSymbol(env));

	Tcl_Obj *obj = Tcl_NewObj();
	int r = Tcl_FSRemoveDirectory(pathPtr.externalAddressValue->contents,
				      recursiveContents,
				      &obj);

	if (r == TCL_OK)
		out->lexemeValue = OkFlag(env);
	else
		out->lexemeValue = CreateString(env, Tcl_GetString(obj));
}

static void clips_Tcl_FSStat(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue pathPtr;
	UDFValue statPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &pathPtr);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &statPtr);

	out->integerValue = CreateInteger(
		env,
		Tcl_FSStat(pathPtr.externalAddressValue->contents,
			   statPtr.externalAddressValue->contents));
}

static void clips_Tcl_GetModificationTimeFromStat(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue statPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &statPtr);

	out->integerValue = CreateInteger(
		env,
		Tcl_GetModificationTimeFromStat(
			statPtr.externalAddressValue->contents));
}

static void clips_Tcl_GetObjResult(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);

	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_GetObjResult(interp.externalAddressValue->contents),
		CLIPS_TCL_INTERP_EXTERNAL_ADDRESS);
}

static void clips_Tcl_GetReturnOptions(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue code;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, SYMBOL_BIT, &code);

	int codeContents;
	if (code.value == OkFlag(env))
		codeContents = TCL_OK;
	else if (code.value == ErrorFlag(env))
		codeContents = TCL_ERROR;
	else
		sscanf(code.lexemeValue->contents, "/%d/", &codeContents);

	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_GetReturnOptions(interp.externalAddressValue->contents,
				     codeContents),
		CLIPS_TCL_INTERP_EXTERNAL_ADDRESS);
}

static void clips_Tcl_GetStdChannel(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue type;

	UDFNthArgument(udfc, 1, SYMBOL_BIT, &type);

	int typeContents = 0;
	const char *p = type.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		assert(strncmp(p, "std", 3) == 0);
		p += 3;
		switch (*p) {
		case 'e':
			assert(strncmp(p, "err", 3) == 0);
			typeContents |= TCL_STDERR;
			p += 3;
			break;
		case 'i':
			assert(strncmp(p, "in", 2) == 0);
			typeContents |= TCL_STDIN;
			p += 2;
			break;
		case 'o':
			assert(strncmp(p, "out", 3) == 0);
			typeContents |= TCL_STDOUT;
			p += 3;
			break;
		default:
			assert(false);
		}
	}

	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_GetStdChannel(typeContents),
		CLIPS_TCL_CHANNEL_EXTERNAL_ADDRESS);
}

static void clips_Tcl_GetString(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue objPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &objPtr);

	out->lexemeValue = CreateString(
		env,
		Tcl_GetString(objPtr.externalAddressValue->contents));
}

static void clips_Tcl_GetStringResult(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);

	out->lexemeValue = CreateString(
		env,
		Tcl_GetStringResult(interp.externalAddressValue->contents));
}

static void clips_Tcl_GetVar(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue varName;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, STRING_BIT, &varName);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int flagsContents = 0;
	const char *p = flags.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		switch (*p) {
		case 'a':
			assert(strncmp(p, "append-value", 12) == 0);
			flagsContents |= TCL_APPEND_VALUE;
			p += 12;
			break;
		case 'g':
			assert(strncmp(p, "global-only", 11) == 0);
			flagsContents |= TCL_GLOBAL_ONLY;
			p += 11;
			break;
		case 'l':
			switch (*p) {
			case 'e':
				assert(strncmp(p, "leave-err-msg", 13) == 0);
				flagsContents |= TCL_LEAVE_ERR_MSG;
				p += 13;
			case 'i':
				assert(strncmp(p, "list-element", 12) == 0);
				flagsContents |= TCL_LIST_ELEMENT;
				p += 12;
			default:
				assert(false);
			}
		default:
			assert(false);
		}
	}

	out->lexemeValue = CreateString(
		env,
		Tcl_GetVar(interp.externalAddressValue->contents,
			   varName.lexemeValue->contents,
			   flagsContents));
}

static void clips_Tcl_GetsObj(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue channel;
	UDFValue lineObjPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &channel);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &lineObjPtr);

	out->integerValue = CreateInteger(
		env,
		Tcl_GetsObj(channel.externalAddressValue->contents,
			    lineObjPtr.externalAddressValue->contents));
}

static void clips_Tcl_IncrRefCount(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue objPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &objPtr);

	Tcl_IncrRefCount((Tcl_Obj *) objPtr.externalAddressValue->contents);
}

static void clips_Tcl_ListObjAppendElement(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue listPtr;
	UDFValue objPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &listPtr);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &objPtr);

	int r = Tcl_ListObjAppendElement(
		interp.externalAddressValue->contents,
		listPtr.externalAddressValue->contents,
		objPtr.externalAddressValue->contents);

	switch (r) {
	case TCL_OK:
		out->lexemeValue = OkFlag(env);
		break;
	case TCL_ERROR:
		out->lexemeValue = ErrorFlag(env);
		break;
	default:
		assert(false);
	}
}

static void clips_Tcl_ListObjGetElements(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue listPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &listPtr);

	int objc;
	Tcl_Obj **objv;
	int r = Tcl_ListObjGetElements(interp.externalAddressValue->contents,
				       listPtr.externalAddressValue->contents,
				       &objc,
				       &objv);

	if (r == TCL_OK) {
		MultifieldBuilder *mb = CreateMultifieldBuilder(env, objc);
		for (int i = 0; i < objc; ++i)
			MBAppendCLIPSExternalAddress(
				mb,
				CreateExternalAddress(
					env,
					objv[i],
					CLIPS_TCL_OBJ_EXTERNAL_ADDRESS));
		out->multifieldValue = MBCreate(mb);
		MBDispose(mb);
	} else {
		out->lexemeValue = ErrorFlag(env);
	}
}

static void clips_Tcl_Merge(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue argv;

	UDFNthArgument(udfc, 1, MULTIFIELD_BIT, &argv);

	int argc = argv.multifieldValue->length;

	size_t argvContentsSize = argc * sizeof (const char *);
	const char **argvContents = genalloc(env, argvContentsSize);
	CLIPSValue *fields = argv.multifieldValue->contents;
	for (int i = 0; i < argc; ++i) {
		assert(fields[i].header->type == STRING_TYPE);
		argvContents[i] = fields[i].lexemeValue->contents;
	}

	char *r = Tcl_Merge(argc, argvContents);
	out->lexemeValue = CreateString(env, r);
	Tcl_Free(r);

	genfree(env, argvContents, argvContentsSize);
}

static void clips_Tcl_NewListObj(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue objv;

	UDFNthArgument(udfc, 1, MULTIFIELD_BIT, &objv);

	int objc = objv.multifieldValue->length;

	size_t objvContentsSize = objc * sizeof (Tcl_Obj *);
	Tcl_Obj **objvContents = genalloc(env, objvContentsSize);
	CLIPSValue *fields = objv.multifieldValue->contents;
	for (int i = 0; i < objc; ++i) {
		assert(fields[i].header->type == EXTERNAL_ADDRESS_TYPE);
		objvContents[i] = fields[i].externalAddressValue->contents;
	}

	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_NewListObj(objc, objvContents),
		CLIPS_TCL_OBJ_EXTERNAL_ADDRESS);

	genfree(env, objvContents, objvContentsSize);
}

static void clips_Tcl_NewObj(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_NewObj(),
		CLIPS_TCL_OBJ_EXTERNAL_ADDRESS);
}

static void clips_Tcl_NewStringObj(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue bytes;

	UDFNthArgument(udfc, 1, STRING_BIT, &bytes);

	size_t length = strlen(bytes.lexemeValue->contents);

	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_NewStringObj(bytes.lexemeValue->contents, length),
		CLIPS_TCL_OBJ_EXTERNAL_ADDRESS);
}

static void clips_Tcl_OpenCommandChannel(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue argv;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, MULTIFIELD_BIT, &argv);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int argc = argv.multifieldValue->length;

	size_t argvContentsSize = argc * sizeof (const char *);
	const char **argvContents = genalloc(env, argvContentsSize);
	CLIPSValue *fields = argv.multifieldValue->contents;
	for (int i = 0; i < argc; ++i) {
		assert(fields[i].header->type == STRING_TYPE);
		argvContents[i] = fields[i].lexemeValue->contents;
	}

	int flagsContents = 0;
	const char *p = flags.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		switch (*p) {
		case 'e':
			assert(strncmp(p, "enforce-mode", 12) == 0);
			flagsContents |= TCL_ENFORCE_MODE;
			p += 12;
			break;
		case 's':
			assert(strncmp(p, "std", 3) == 0);
			p += 3;
			switch (*p) {
			case 'e':
				assert(strncmp(p, "err", 3) == 0);
				flagsContents |= TCL_STDERR;
				p += 3;
				break;
			case 'i':
				assert(strncmp(p, "in", 2) == 0);
				flagsContents |= TCL_STDIN;
				p += 2;
				break;
			case 'o':
				assert(strncmp(p, "out", 3) == 0);
				flagsContents |= TCL_STDOUT;
				p += 3;
				break;
			default:
				assert(false);
			}
			break;
		default:
			assert(false);
		}
	}

	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_OpenCommandChannel(interp.externalAddressValue->contents,
				       argc,
				       argvContents,
				       flagsContents),
		CLIPS_TCL_CHANNEL_EXTERNAL_ADDRESS);

	genfree(env, argvContents, argvContentsSize);
}

static void clips_Tcl_OpenTcpClient(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue port;
	UDFValue host;
	UDFValue myaddr;
	UDFValue myport;
	UDFValue async;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, INTEGER_BIT, &port);
	UDFNthArgument(udfc, 3, STRING_BIT, &host);
	UDFNthArgument(udfc, 4, BOOLEAN_BIT | STRING_BIT, &myaddr);
	UDFNthArgument(udfc, 5, INTEGER_BIT, &myport);
	UDFNthArgument(udfc, 6, BOOLEAN_BIT, &async);

	bool asyncContents = (async.value == TrueSymbol(env));

	const char *myaddrContents;
	if (myaddr.header->type == SYMBOL_TYPE) {
		assert(myaddr.value == FalseSymbol(env));
		myaddrContents = NULL;
	} else {
		myaddrContents = myaddr.lexemeValue->contents;
	}

	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_OpenTcpClient(interp.externalAddressValue->contents,
				  port.integerValue->contents,
				  host.lexemeValue->contents,
				  myaddrContents,
				  myport.integerValue->contents,
				  asyncContents),
		CLIPS_TCL_CHANNEL_EXTERNAL_ADDRESS);
}

static void clips_Tcl_Sleep(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue ms;

	UDFNthArgument(udfc, 1, INTEGER_BIT, &ms);

	Tcl_Sleep(ms.integerValue->contents);
}

static void clips_Tcl_TcpAcceptProc(
	ClientData clientData, Tcl_Channel channel, char *hostName, int port)
{
	Environment *env = ((void **) clientData)[0];
	CLIPSLexeme *functionValue = ((void **) clientData)[1];

	const int argc = 4;
	void *argv[argc];
	argv[0] = ((void **) clientData)[2];
	argv[1] = CreateExternalAddress(
		env, channel, CLIPS_TCL_CHANNEL_EXTERNAL_ADDRESS);
	argv[2] = CreateString(env, hostName);
	argv[3] = CreateInteger(env, port);

	Expression reference;
	GetFunctionReference(env, functionValue->contents, &reference);
	ExpressionInstall(env, &reference);

	Expression *p = NULL;
	for (int i = 0; i < argc; ++i) {
		Expression *c = GenConstant(
			env, ((TypeHeader *) argv[i])->type, argv[i]);
		if (p == NULL)
			reference.argList = c;
		else
			p->nextArg = c;
		p = c;
		ExpressionInstall(env, p);
	}

	UDFValue returnValue;
	EvaluateExpression(env, &reference, &returnValue);
	ExpressionDeinstall(env, &reference);
}

static void clips_Tcl_OpenTcpServerCloseProc(
	ClientData clientData)
{
	Tcl_Free(clientData);
}

static void clips_Tcl_OpenTcpServer(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue port;
	UDFValue myaddr;
	UDFValue proc;
	UDFValue clientData;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, INTEGER_BIT, &port);
	UDFNthArgument(udfc, 3, BOOLEAN_BIT | STRING_BIT, &myaddr);
	UDFNthArgument(udfc, 4, SYMBOL_BIT, &proc);
	UDFNthArgument(udfc, 5, ANY_TYPE_BITS, &clientData);

	const char *myaddrContents;
	if (myaddr.header->type == SYMBOL_TYPE) {
		assert(myaddr.value == FalseSymbol(env));
		myaddrContents = NULL;
	} else {
		myaddrContents = myaddr.lexemeValue->contents;
	}

	size_t clientDataContentsSize = 3 * sizeof (void *);
	void **clientDataContents = (void *) Tcl_Alloc(clientDataContentsSize);
	clientDataContents[0] = env;
	clientDataContents[1] = proc.value;
	clientDataContents[2] = clientData.value;

	Tcl_Channel r = Tcl_OpenTcpServer(
		interp.externalAddressValue->contents,
		port.integerValue->contents,
		myaddrContents,
		clips_Tcl_TcpAcceptProc,
		clientDataContents);

	out->externalAddressValue = CreateExternalAddress(
		env, r, CLIPS_TCL_CHANNEL_EXTERNAL_ADDRESS);

	Tcl_CreateCloseHandler(r,
			       clips_Tcl_OpenTcpServerCloseProc,
			       clientDataContents);
}

static void clips_Tcl_RegisterChannel(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue channel;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &channel);

	Tcl_RegisterChannel(interp.externalAddressValue->contents,
			    channel.externalAddressValue->contents);
}

static void clips_Tcl_SetChannelOption(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue channel;
	UDFValue optionName;
	UDFValue newValue;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &channel);
	UDFNthArgument(udfc, 3, STRING_BIT, &optionName);
	UDFNthArgument(udfc, 4, STRING_BIT, &newValue);

	int r = Tcl_SetChannelOption(interp.externalAddressValue->contents,
				     channel.externalAddressValue->contents,
				     optionName.lexemeValue->contents,
				     newValue.lexemeValue->contents);

	switch (r) {
	case TCL_OK:
		out->lexemeValue = OkFlag(env);
		break;
	case TCL_ERROR:
		out->lexemeValue = ErrorFlag(env);
		break;
	default:
		assert(false);
	}
}

static void clips_Tcl_SplitList(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue list;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, STRING_BIT, &list);

	int argc;
	const char **argv;
	int r = Tcl_SplitList(interp.externalAddressValue->contents,
			      list.lexemeValue->contents,
			      &argc,
			      &argv);

	if (r == TCL_OK) {
		MultifieldBuilder *mb = CreateMultifieldBuilder(env, argc);
		for (int i = 0; i < argc; ++i)
			MBAppendString(mb, argv[i]);
		out->multifieldValue = MBCreate(mb);
		MBDispose(mb);
		Tcl_Free((void *) argv);
	} else {
		out->lexemeValue = ErrorFlag(env);
	}
}

static void clips_Tcl_WriteChars(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue channel;
	UDFValue charBuf;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &channel);
	UDFNthArgument(udfc, 2, STRING_BIT, &charBuf);

	size_t bytesToWrite = strlen(charBuf.lexemeValue->contents);

	out->integerValue = CreateInteger(
		env,
		Tcl_WriteChars(channel.externalAddressValue->contents,
			       charBuf.lexemeValue->contents,
			       bytesToWrite));
}

static void clips_Tcl_WriteObj(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue channel;
	UDFValue writeObjPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &channel);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &writeObjPtr);

	out->integerValue = CreateInteger(
		env,
		Tcl_WriteObj(channel.externalAddressValue->contents,
			     writeObjPtr.externalAddressValue->contents));
}

static void clips_tcl_EnvironmentCleanupFunction(Environment *env)
{
	ReleaseLexeme(env, OkFlag(env));
	ReleaseLexeme(env, ErrorFlag(env));
	ReleaseLexeme(env, ZeroFlag(env));
	ReleaseLexeme(env, MinusOneFlag(env));
}

void UserFunctions(Environment *env)
{
	AllocateEnvironmentData(
		env, OK_FLAG_DATA_ID, sizeof (CLIPSLexeme *), NULL);
	AllocateEnvironmentData(
		env, ERROR_FLAG_DATA_ID, sizeof (CLIPSLexeme *), NULL);
	AllocateEnvironmentData(
		env, ZERO_FLAG_DATA_ID, sizeof (CLIPSLexeme *), NULL);
	AllocateEnvironmentData(
		env, MINUS_ONE_FLAG_DATA_ID, sizeof (CLIPSLexeme *), NULL);

	RetainLexeme(env, OkFlag(env) = CreateSymbol(env, "/ok/"));
	RetainLexeme(env, ErrorFlag(env) = CreateSymbol(env, "/error/"));
	RetainLexeme(env, ZeroFlag(env) = CreateSymbol(env, "/0/"));
	RetainLexeme(env, MinusOneFlag(env) = CreateSymbol(env, "/-1/"));

	// According to manual, priority -2000 to 2000 are reserved by CLIPS.
	AddEnvironmentCleanupFunction(
		env,
		"clips_tcl_EnvironmentCleanupFunction",
		clips_tcl_EnvironmentCleanupFunction,
		5000);


	AddUDF(env,
	       "tcl-alloc-stat-buf",
	       "e", 0, 0, "",
	       clips_Tcl_AllocStatBuf,
	       "clips_Tcl_AllocStatBuf",
	       NULL);

	AddUDF(env,
	       "tcl-close",
	       "l", 2, 2, ";e;e",
	       clips_Tcl_Close,
	       "clips_Tcl_Close",
	       NULL);

	AddUDF(env,
	       "tcl-concat",
	       "s", 1, 1, ";m",
	       clips_Tcl_Concat,
	       "clips_Tcl_Concat",
	       NULL);

	AddUDF(env,
	       "tcl-create-interp",
	       "e", 0, 0, "",
	       clips_Tcl_CreateInterp,
	       "clips_Tcl_CreateInterp",
	       NULL);

	AddUDF(env,
	       "tcl-decr-ref-count",
	       "v", 1, 1, ";e",
	       clips_Tcl_DecrRefCount,
	       "clips_Tcl_DecrRefCount",
	       NULL);

	AddUDF(env,
	       "tcl-delete-interp",
	       "v", 1, 1, ";e",
	       clips_Tcl_DeleteInterp,
	       "clips_Tcl_DeleteInterp",
	       NULL);

	AddUDF(env,
	       "tcl-do-one-event",
	       "l", 1, 1, ";y",
	       clips_Tcl_DoOneEvent,
	       "clips_Tcl_DoOneEvent",
	       NULL);

	AddUDF(env,
	       "tcl-eval-ex",
	       "y", 3, 3, ";e;s;y",
	       clips_Tcl_EvalEx,
	       "clips_Tcl_EvalEx",
	       NULL);

	AddUDF(env,
	       "tcl-eval-obj-ex",
	       "y", 3, 3, ";e;e;y",
	       clips_Tcl_EvalObjEx,
	       "clips_Tcl_EvalObjEx",
	       NULL);

	AddUDF(env,
	       "tcl-eval-objv",
	       "y", 3, 3, ";e;m;y",
	       clips_Tcl_EvalObjv,
	       "clips_Tcl_EvalObjv",
	       NULL);

	AddUDF(env,
	       "tcl-flush",
	       "b", 1, 1, ";e",
	       clips_Tcl_Flush,
	       "clips_Tcl_Flush",
	       NULL);

	AddUDF(env,
	       "tcl-fs-create-directory",
	       "y", 1, 1, ";e",
	       clips_Tcl_FSCreateDirectory,
	       "clips_Tcl_FSCreateDirectory",
	       NULL);

	AddUDF(env,
	       "tcl-fs-remove-directory",
	       "sy", 2, 2, ";e;b",
	       clips_Tcl_FSRemoveDirectory,
	       "clips_Tcl_FSRemoveDirectory",
	       NULL);

	AddUDF(env,
	       "tcl-fs-stat",
	       "l", 2, 2, ";e;e",
	       clips_Tcl_FSStat,
	       "clips_Tcl_FSStat",
	       NULL);

	AddUDF(env,
	       "tcl-get-modification-time-from-stat",
	       "l", 1, 1, ";e",
	       clips_Tcl_GetModificationTimeFromStat,
	       "clips_Tcl_GetModificationTimeFromStat",
	       NULL);

	AddUDF(env,
	       "tcl-get-obj-result",
	       "e", 1, 1, ";e",
	       clips_Tcl_GetObjResult,
	       "clips_Tcl_GetObjResult",
	       NULL);

	AddUDF(env,
	       "tcl-get-return-options",
	       "e", 2, 2, ";e;y",
	       clips_Tcl_GetReturnOptions,
	       "clips_Tcl_GetReturnOptions",
	       NULL);

	AddUDF(env,
	       "tcl-get-std-channel",
	       "e", 1, 1, ";y",
	       clips_Tcl_GetStdChannel,
	       "clips_Tcl_GetStdChannel",
	       NULL);

	AddUDF(env,
	       "tcl-get-string",
	       "s", 1, 1, ";e",
	       clips_Tcl_GetString,
	       "clips_Tcl_GetString",
	       NULL);

	AddUDF(env,
	       "tcl-get-string-result",
	       "s", 1, 1, ";e",
	       clips_Tcl_GetStringResult,
	       "clips_Tcl_GetStringResult",
	       NULL);

	AddUDF(env,
	       "tcl-get-var",
	       "s", 3, 3, ";e;s;y",
	       clips_Tcl_GetVar,
	       "clips_Tcl_GetVar",
	       NULL);

	AddUDF(env,
	       "tcl-gets-obj",
	       "l", 2, 2, ";e;e",
	       clips_Tcl_GetsObj,
	       "clips_Tcl_GetsObj",
	       NULL);

	AddUDF(env,
	       "tcl-incr-ref-count",
	       "v", 1, 1, ";e",
	       clips_Tcl_IncrRefCount,
	       "clips_Tcl_IncrRefCount",
	       NULL);

	AddUDF(env,
	       "tcl-list-obj-append-element",
	       "y", 3, 3, ";e;e;e",
	       clips_Tcl_ListObjAppendElement,
	       "clips_Tcl_ListObjAppendElement",
	       NULL);

	AddUDF(env,
	       "tcl-list-obj-get-elements",
	       "my", 2, 2, ";e;e",
	       clips_Tcl_ListObjGetElements,
	       "clips_Tcl_ListObjGetElements",
	       NULL);

	AddUDF(env,
	       "tcl-merge",
	       "s", 1, 1, ";m",
	       clips_Tcl_Merge,
	       "clips_Tcl_Merge",
	       NULL);

	AddUDF(env,
	       "tcl-new-list-obj",
	       "e", 1, 1, ";m",
	       clips_Tcl_NewListObj,
	       "clips_Tcl_NewListObj",
	       NULL);

	AddUDF(env,
	       "tcl-new-obj",
	       "e", 0, 0, "",
	       clips_Tcl_NewObj,
	       "clips_Tcl_NewObj",
	       NULL);

	AddUDF(env,
	       "tcl-new-string-obj",
	       "e", 1, 1, ";s",
	       clips_Tcl_NewStringObj,
	       "clips_Tcl_NewStringObj",
	       NULL);

	AddUDF(env,
	       "tcl-open-command-channel",
	       "e", 3, 3, ";e;m;y",
	       clips_Tcl_OpenCommandChannel,
	       "clips_Tcl_OpenCommandChannel",
	       NULL);

	AddUDF(env,
	       "tcl-open-tcp-client",
	       "e", 6, 6, ";e;l;s;bs;l;b",
	       clips_Tcl_OpenTcpClient,
	       "clips_Tcl_OpenTcpClient",
	       NULL);

	AddUDF(env,
	       "tcl-open-tcp-server",
	       "e", 5, 5, ";e;l;bs;y;*",
	       clips_Tcl_OpenTcpServer,
	       "clips_Tcl_OpenTcpServer",
	       NULL);

	AddUDF(env,
	       "tcl-register-channel",
	       "v", 2, 2, ";e;e",
	       clips_Tcl_RegisterChannel,
	       "clips_Tcl_RegisterChannel",
	       NULL);

	AddUDF(env,
	       "tcl-set-channel-option",
	       "y", 4, 4, ";e;e;s;s",
	       clips_Tcl_SetChannelOption,
	       "clips_Tcl_SetChannelOption",
	       NULL);

	AddUDF(env,
	       "tcl-sleep",
	       "v", 1, 1, ";l",
	       clips_Tcl_Sleep,
	       "clips_Tcl_Sleep",
	       NULL);

	AddUDF(env,
	       "tcl-split-list",
	       "my", 2, 2, ";e;s",
	       clips_Tcl_SplitList,
	       "clips_Tcl_SplitList",
	       NULL);

	AddUDF(env,
	       "tcl-write-chars",
	       "l", 2, 2, ";e;s",
	       clips_Tcl_WriteChars,
	       "clips_Tcl_WriteChars",
	       NULL);

	AddUDF(env,
	       "tcl-write-obj",
	       "l", 2, 2, ";e;e",
	       clips_Tcl_WriteObj,
	       "clips_Tcl_WriteObj",
	       NULL);
}
