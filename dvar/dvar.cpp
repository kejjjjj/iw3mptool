#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "dvar.hpp"
#include "utils/engine.hpp"
#include "utils/hook.hpp"

#include <iostream>
#include <chrono>
#include <format>

dvar_s* Dvar_FindMalleableVar(const char* name)
{
    constexpr auto addr = 0x56b5d0;
    dvar_s* r = 0;
    __asm
    {
        mov edi, name;
        call addr;
        mov r, eax;
    }

    return r;
}

dvar_s* Dvar_RegisterNew(const char* name, dvar_type _type, int flags, const char* description, dvar_value defaultValue, dvar_limits domain)
{
    return Engine::call<dvar_s*>(0x056C130, name, _type, flags, description, defaultValue, domain);
}
dvar_s* Dvar_RegisterFloat(const char* name, float value, float min, float max, int flags, const char* description)
{
    return Engine::call<dvar_s*>(0x56C460, name, value, min, max, flags, description);
}
dvar_s* Dvar_RegisterInt(const char* name, int value, int min, int max, int flags, const char* description)
{
    return Engine::call<dvar_s*>(0x56C410, name, value, min, max, flags, description);
}
dvar_s* Dvar_RegisterBool(const char* name, int flags, bool value, const char* description)
{
    dvar_s* result = 0;

    __asm {
        mov al, value;
        push description;
        push flags;
        push name;
        mov esi, 0x56C3C0;
        call esi;
        add esp, 0xC;
        mov result, eax;
    }

    return result;

}
dvar_s* Dvar_RegisterEnum(const char* name, const char** valueList, int enumSize, int defaultIndex, int flags, const char* description)
{
    return Engine::call<dvar_s*>(0x56C130, name, dvar_type::enumeration, flags, description, defaultIndex, 0, 0, 0, enumSize, valueList);
}
char* Dvar_GetVariantString(const char* v)
{
    __asm
    {
        mov edi, v;
        mov esi, 0x56B6E0;
        call esi;
    }
}

__declspec(naked) void CG_SetClientDvarFromServerASM()
{
    __asm
    {
        push ebx;
        mov ebx, [esp + 8h];

        push ebx;
        push edi;
        push eax;

        call CG_SetClientDvarFromServer;
        add esp, 0xC;

        pop ebx;
        retn;
    }
}
void CG_SetClientDvarFromServer(const char* dvarname, const char* value, [[maybe_unused]]cg_s* _cgs)
{

    if (_stricmp(dvarname, "cg_objectiveText"))
    {
        if (_stricmp(dvarname, "hud_drawHud"))
        {
            if (_stricmp(dvarname, "g_scriptMainMenu"))
            {
                //this is ok to be called when offline
                const auto _dvar = Dvar_FindMalleableVar("sec_executeCommands");
                if (Dvar_FindMalleableVar("sv_running")->current.enabled || _dvar && _dvar->current.enabled) {
                    constexpr auto f = 0x56D0A0;
                    __asm
                    {
                        push 0;
                        push value;
                        mov eax, dvarname;
                        call f;
                        add esp, 8;
                    }

                    return;

                }

                std::cout << "Server tried to execute: " << std::quoted(std::format("{} {}", dvarname, value)) << '\n';
                return;
            }
            else
            {
                strncpy_s(cgs->scriptMainMenu, value, 0xFFu);
                cgs->scriptMainMenu[255] = 0;
            }
        }
        else
        {
            cgs->drawHud = std::atol(value);
        }
    }
    else
    {
        strncpy_s(cgs->objectiveText, value, 0x3FFu);
        cgs->objectiveText[1023] = 0;
    }

    return;
}

//char* Info_ValueForKey([[maybe_unused]]volatile const char* string, char* result)
//{
//    if (Dvar_FindMalleableVar("sv_running")->current.enabled || CL_ConnectionState() < CA_SENDINGSTATS)
//        return result;
//
//    //if (string)
//    //    std::cout << "key: " << string << '\n';
//
//    //if (result)
//    //    std::cout << "value: " << result << '\n';
//    //
//    //std::cout << '\n';
//
//    return nullptr;
//}
//
//__declspec(naked) void Info_ValueForKeyASM()
//{
//    constexpr static auto failure = 0x0571F94;
//    constexpr static auto _exit = 0x571FA1;
//    __asm
//    {
//        add esp, 4;
//        test eax, eax;
//        jz success;
//
//        //continue
//        jmp failure;
//
//    success:
//        push ebp;
//        push edx;
//
//        call Info_ValueForKey;
//        add esp, 8h;
//
//        jmp _exit;
//        retn;
//    }
//}