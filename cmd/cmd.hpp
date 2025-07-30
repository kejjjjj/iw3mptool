#pragma once

struct cmd_function_s;

[[maybe_unused]] cmd_function_s* Cmd_FindCommand(const char* name);
[[maybe_unused]] cmd_function_s* Cmd_AddCommand(const char* cmdname, void(__cdecl* function)());
[[maybe_unused]] cmd_function_s* Cmd_RemoveCommand(const char* cmdname);

void CBuf_Addtext(const char* text);
void CBuf_ExecuteBuffer(const char* text);