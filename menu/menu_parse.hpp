#pragma once

void __cdecl Script_AddTextWrapper(int clientNum, int controllerIndex, const char* text);
int __cdecl Script_ExecHandler(int localClientNum, int item, const char** args, int(__cdecl* textCallback)(int, int, char*));

void Script_ConditionalExecHandler(
	const char** args,
	int localClientNum,
	int(__cdecl* shouldExec)(char*, char*),
	int(__cdecl* textCallback)(int, int, char*));
