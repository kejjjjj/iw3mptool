#include "menu_parse.hpp"
#include "dvar/dvar.hpp"
#include "com/com_channel.hpp"
#include "cmd/cmd.hpp"
#include <string.h>

static int String_Parse(const char** p, char* _out, int len)
{
	__asm
	{
		mov eax, p;
		push len;
		push _out;
		mov esi, 0x54B510;
		call esi;
		add esp, 8;
	}
}

#pragma optimize("", off)
void __cdecl Script_AddTextWrapper([[maybe_unused]] int clientNum, [[maybe_unused]] int controllerIndex, const char* text)
{
    const auto _dvar = Dvar_FindMalleableVar("sec_executeCommands");
    if (Dvar_FindMalleableVar("sv_running")->current.enabled || _dvar && _dvar->current.enabled) {
        return CBuf_Addtext(text);

    }
	Com_Printf("^1the server tried to execute: %s\n", text);

}

int __cdecl Script_ExecHandler([[maybe_unused]] int localClientNum, [[maybe_unused]] int item, const char** args, [[maybe_unused]] int(__cdecl* textCallback)(int, int, char*))
{
	char val[1024];

	auto result = String_Parse(args, val, 1023);
	if (result)
	{
		strncat_s(val, "\n", 1);

		const auto _dvar = Dvar_FindMalleableVar("sec_executeCommands");
		if (Dvar_FindMalleableVar("sv_running")->current.enabled || _dvar && _dvar->current.enabled)
			return textCallback(localClientNum, item, val);

		Com_Printf("^1the server tried to execute: %s\n", val);
		return 1;
	}
	return result;

}

void Script_ConditionalExecHandler(
	const char** args,
	[[maybe_unused]] int localClientNum,
	[[maybe_unused]] int(__cdecl* shouldExec)(char*, char*),
	[[maybe_unused]] int(__cdecl* textCallback)(int, int, char*))
{

	__asm mov args, esi;

	char* VariantString; // eax
	char command[1024]; // [esp+4h] [ebp-C00h] BYREF
	char dvarName[1024]; // [esp+404h] [ebp-800h] BYREF
	char testValue[1024]; // [esp+804h] [ebp-400h] BYREF

	if (String_Parse(args, dvarName, 1024))
	{
		if (String_Parse(args, testValue, 1024))
		{
			if (String_Parse(args, command, 1023))
			{
				const auto _dvar = Dvar_FindMalleableVar("sec_executeCommands");
				if (Dvar_FindMalleableVar("sv_running")->current.enabled || _dvar && _dvar->current.enabled) {
					VariantString = Dvar_GetVariantString(dvarName);
					if (shouldExec(VariantString, testValue))
					{
						strncat_s(command, "\n", 1);
						textCallback(localClientNum, localClientNum, command);
					}
				}
				else {
					Com_Printf("^1the server tried to execute: %s\n", command);
				}
			}
		}
	}
}
#pragma optimize("", on)
