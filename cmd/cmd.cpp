#include "cmd.hpp"
#include "cod4x/cod4x.hpp"
#include "utils/engine.hpp"
#include "cg/cg_local.hpp"
#include <cg/cg_offsets.hpp>

cmd_function_s* Cmd_FindCommand(const char* name)
{
    static const DWORD fnc = 0x4F9950;
    __asm
    {
        mov esi, name;
        call fnc;
    }
}
cmd_function_s* Cmd_AddCommand(const char* cmdname, void(__cdecl* function)())
{
    if (CoD4X::Get())
        return Engine::call<cmd_function_s*>(CoD4X::Get() + 0x2116C, cmdname, function);

    cmd_function_s* cmd = Cmd_FindCommand(cmdname);

    if (cmd)
        return cmd;


    static cmd_function_s cmds[64];
    static std::uint8_t index = 0;
    cmd = &cmds[index++];
    __asm {
        push function;
        mov edi, cmd;
        mov eax, cmdname;
        mov esi, 0x4F99A0;
        call esi;
        add esp, 4;
    }

    return cmd_functions;
}
cmd_function_s* Cmd_RemoveCommand(const char* cmdname)
{
    __asm
    {
        push cmdname;
        mov esi, 0x4F99E0;
        call esi;
        add esp, 0x4;
    }

    return 0;
}

void CBuf_Addtext(const char* text)
{
    __asm {
        mov eax, text;
        push eax;
        mov ecx, 0;
        push ecx;
        mov esi, 0x4F8D90;
        call esi;
        add esp, 0x8;
    }
}
void CBuf_ExecuteBuffer(const char* text)
{
    __asm
    {
        push text;
        push 0;
        push 0;
        mov esi, 0x4F91C0;
        call esi;
        add esp, 0xC;
    }
}