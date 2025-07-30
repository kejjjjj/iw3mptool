#include "scr_functions.hpp"

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"

char* Scr_GetString(int string)
{
    return (char*)&gScrMemTreePub->mt_buffer[12 * string + 4];
}
int SL_GetStringOfSize(const std::string& string)
{
    const auto Unknown = 1;
    const auto _length = string.length();
    auto _str = string.data();

    auto result = 0;

    _asm {
        push	_length
        push	Unknown
        push	_str
        mov	eax, 0x54CEC0
        call	eax
        add	esp, 0xC
        mov result, eax;
    }
    return result;
}