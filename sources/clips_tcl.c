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

enum {
	CLIPS_TCL_CHANNEL_EXTERNAL_ADDRESS = C_POINTER_EXTERNAL_ADDRESS + 1,
	CLIPS_TCL_INTERP_EXTERNAL_ADDRESS,
	CLIPS_TCL_OBJ_EXTERNAL_ADDRESS,
	CLIPS_TCL_STAT_BUF_EXTERNAL_ADDRESS
};

static void clips_tcl_AllocStatBuf(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_AllocStatBuf(),
		CLIPS_TCL_STAT_BUF_EXTERNAL_ADDRESS);
}

static void clips_tcl_Close(
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

static void clips_tcl_CreateInterp(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_CreateInterp(),
		CLIPS_TCL_INTERP_EXTERNAL_ADDRESS);
}

static void clips_tcl_DecrRefCount(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue objPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &objPtr);

	Tcl_DecrRefCount((Tcl_Obj *) objPtr.externalAddressValue->contents);
}

static void clips_tcl_DeleteInterp(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);

	Tcl_DeleteInterp(interp.externalAddressValue->contents);
}

static void clips_tcl_EvalObjEx(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue objPtr;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, EXTERNAL_ADDRESS_BIT, &objPtr);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int flags_value = 0;
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
			flags_value |= TCL_EVAL_DIRECT;
			p += 6;
			break;
		case 'g':
			assert(strncmp(p, "global", 6) == 0);
			flags_value |= TCL_EVAL_GLOBAL;
			p += 6;
			break;
		default:
			assert(false);
		}
	}

	out->integerValue = CreateInteger(
		env,
		Tcl_EvalObjEx(interp.externalAddressValue->contents,
			      objPtr.externalAddressValue->contents,
			      flags_value));
}

static void clips_tcl_EvalObjv(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue objv;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, MULTIFIELD_BIT, &objv);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int objc = objv.multifieldValue->length;

	size_t objv_value_size = objc * sizeof (Tcl_Obj *);
	Tcl_Obj **objv_value = genalloc(env, objv_value_size);
	CLIPSValue *fields = objv.multifieldValue->contents;
	for (int i = 0; i < objc; ++i)
		objv_value[i] = fields[i].externalAddressValue->contents;

	int flags_value = 0;
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
			flags_value |= TCL_EVAL_DIRECT;
			p += 6;
			break;
		case 'g':
			assert(strncmp(p, "global", 6) == 0);
			flags_value |= TCL_EVAL_GLOBAL;
			p += 6;
			break;
		default:
			assert(false);
		}
	}

	out->integerValue = CreateInteger(
		env,
		Tcl_EvalObjv(interp.externalAddressValue->contents,
			     objc,
			     objv_value,
			     flags_value));

	genfree(env, objv_value, objv_value_size);
}

static void clips_tcl_Flush(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue channel;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &channel);

	out->integerValue = CreateInteger(
		env,
		Tcl_Flush(channel.externalAddressValue->contents));
}

static void clips_tcl_FSStat(
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

static void clips_tcl_GetModificationTimeFromStat(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue statPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &statPtr);

	out->integerValue = CreateInteger(
		env,
		Tcl_GetModificationTimeFromStat(
			statPtr.externalAddressValue->contents));
}

static void clips_tcl_GetString(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue objPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &objPtr);

	out->lexemeValue = CreateString(
		env,
		Tcl_GetString(objPtr.externalAddressValue->contents));
}

static void clips_tcl_GetVar(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue varName;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, STRING_BIT, &varName);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int flags_value = 0;
	const char *p = flags.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		switch (*p) {
		case 'a':
			assert(strncmp(p, "append-value", 12) == 0);
			flags_value |= TCL_APPEND_VALUE;
			p += 12;
			break;
		case 'g':
			assert(strncmp(p, "global-only", 11) == 0);
			flags_value |= TCL_GLOBAL_ONLY;
			p += 11;
			break;
		case 'l':
			switch (*p) {
			case 'e':
				assert(strncmp(p, "leave-err-msg", 13) == 0);
				flags_value |= TCL_LEAVE_ERR_MSG;
				p += 13;
			case 'i':
				assert(strncmp(p, "list-element", 12) == 0);
				flags_value |= TCL_LIST_ELEMENT;
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
			   flags_value));
}

static void clips_tcl_GetsObj(
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

static void clips_tcl_IncrRefCount(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue objPtr;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &objPtr);

	Tcl_IncrRefCount((Tcl_Obj *) objPtr.externalAddressValue->contents);
}

static void clips_tcl_NewObj(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	out->externalAddressValue = CreateExternalAddress(
		env,
		Tcl_NewObj(),
		CLIPS_TCL_OBJ_EXTERNAL_ADDRESS);
}

static void clips_tcl_NewStringObj(
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

static void clips_tcl_OpenCommandChannel(
	Environment *env, UDFContext *udfc, UDFValue *out)
{
	UDFValue interp;
	UDFValue argv;
	UDFValue flags;

	UDFNthArgument(udfc, 1, EXTERNAL_ADDRESS_BIT, &interp);
	UDFNthArgument(udfc, 2, MULTIFIELD_BIT, &argv);
	UDFNthArgument(udfc, 3, SYMBOL_BIT, &flags);

	int argc = argv.multifieldValue->length;

	size_t argv_value_size = argc * sizeof (const char *);
	const char **argv_value = genalloc(env, argv_value_size);
	CLIPSValue *fields = argv.multifieldValue->contents;
	for (int i = 0; i < argc; ++i)
		argv_value[i] = fields[i].lexemeValue->contents;

	int flags_value = 0;
	const char *p = flags.lexemeValue->contents;
	while (true) {
		assert(*p == '/');

		if (!*++p)
			break;

		switch (*p) {
		case 'e':
			assert(strncmp(p, "enforce-mode", 12) == 0);
			flags_value |= TCL_ENFORCE_MODE;
			p += 12;
			break;
		case 's':
			assert(strncmp(p, "std", 3) == 0);
			p += 3;
			switch (*p) {
			case 'e':
				assert(strncmp(p, "err", 3) == 0);
				flags_value |= TCL_STDERR;
				p += 3;
				break;
			case 'i':
				assert(strncmp(p, "in", 2) == 0);
				flags_value |= TCL_STDIN;
				p += 2;
				break;
			case 'o':
				assert(strncmp(p, "out", 3) == 0);
				flags_value |= TCL_STDOUT;
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
				       argv_value,
				       flags_value),
		CLIPS_TCL_CHANNEL_EXTERNAL_ADDRESS);

	genfree(env, argv_value, argv_value_size);
}

static void clips_tcl_WriteChars(
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

void UserFunctions(Environment *env)
{
	AddUDF(env,
	       "tcl-alloc-stat-buf",
	       "e", 0, 0, "",
	       clips_tcl_AllocStatBuf,
	       "clips_tcl_AllocStatBuf",
	       NULL);

	AddUDF(env,
	       "tcl-close",
	       "l", 2, 2, ";e;e",
	       clips_tcl_Close,
	       "clips_tcl_Close",
	       NULL);

	AddUDF(env,
	       "tcl-create-interp",
	       "e", 0, 0, "",
	       clips_tcl_CreateInterp,
	       "clips_tcl_CreateInterp",
	       NULL);

	AddUDF(env,
	       "tcl-decr-ref-count",
	       "v", 1, 1, ";e",
	       clips_tcl_DecrRefCount,
	       "clips_tcl_DecrRefCount",
	       NULL);

	AddUDF(env,
	       "tcl-delete-interp",
	       "v", 1, 1, ";e",
	       clips_tcl_DeleteInterp,
	       "clips_tcl_DeleteInterp",
	       NULL);

	AddUDF(env,
	       "tcl-eval-obj-ex",
	       "l", 3, 3, ";e;e;y",
	       clips_tcl_EvalObjEx,
	       "clips_tcl_EvalObjEx",
	       NULL);

	AddUDF(env,
	       "tcl-eval-objv",
	       "l", 3, 3, ";e;m;y",
	       clips_tcl_EvalObjv,
	       "clips_tcl_EvalObjv",
	       NULL);

	AddUDF(env,
	       "tcl-flush",
	       "l", 1, 1, ";e",
	       clips_tcl_Flush,
	       "clips_tcl_Flush",
	       NULL);

	AddUDF(env,
	       "tcl-fs-stat",
	       "l", 2, 2, ";e;e",
	       clips_tcl_FSStat,
	       "clips_tcl_FSStat",
	       NULL);

	AddUDF(env,
	       "tcl-get-modification-time-from-stat",
	       "l", 1, 1, ";e",
	       clips_tcl_GetModificationTimeFromStat,
	       "clips_tcl_GetModificationTimeFromStat",
	       NULL);

	AddUDF(env,
	       "tcl-get-string",
	       "s", 1, 1, ";e",
	       clips_tcl_GetString,
	       "clips_tcl_GetString",
	       NULL);

	AddUDF(env,
	       "tcl-get-var",
	       "s", 3, 3, ";e;s;y",
	       clips_tcl_GetVar,
	       "clips_tcl_GetVar",
	       NULL);

	AddUDF(env,
	       "tcl-gets-obj",
	       "l", 2, 2, ";e;e",
	       clips_tcl_GetsObj,
	       "clips_tcl_GetsObj",
	       NULL);

	AddUDF(env,
	       "tcl-incr-ref-count",
	       "v", 1, 1, ";e",
	       clips_tcl_IncrRefCount,
	       "clips_tcl_IncrRefCount",
	       NULL);

	AddUDF(env,
	       "tcl-new-obj",
	       "e", 0, 0, "",
	       clips_tcl_NewObj,
	       "clips_tcl_NewObj",
	       NULL);

	AddUDF(env,
	       "tcl-new-string-obj",
	       "e", 1, 1, ";s",
	       clips_tcl_NewStringObj,
	       "clips_tcl_NewStringObj",
	       NULL);

	AddUDF(env,
	       "tcl-open-command-channel",
	       "e", 3, 3, ";e;m;y",
	       clips_tcl_OpenCommandChannel,
	       "clips_tcl_OpenCommandChannel",
	       NULL);

	AddUDF(env,
	       "tcl-write-chars",
	       "l", 2, 2, ";e;s",
	       clips_tcl_WriteChars,
	       "clips_tcl_WriteChars",
	       NULL);
}
