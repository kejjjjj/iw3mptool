

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cg_memory.hpp"

#include "cod4x/cod4x.hpp"
#include "utils/engine.hpp"
#include "utils/hook.hpp"

#include <array>
#include <iostream>
#include <format>

template<MemoryAddress_t Mem>
struct CGameMemoryTweak
{
    Mem address{};
    std::string tweaked{};
    std::string original{};

};

using namespace std::string_literals;
std::array<CGameMemoryTweak<std::int32_t>, 2> tweaks = {
    CGameMemoryTweak{.address = 0x6496DB, .tweaked = "\xEB\x00\xBA\xF0\xF5"s, .original = "\x74\x14"s }, //jmp -> jnz in RB_EndSceneRendering
    CGameMemoryTweak{.address = 0x434200, .tweaked = "\x51\x53\x8B\x5C\x24"s, .original = "\x51\x53\x8B\x5C\x24"s } //remove any possible hooks from CG_DObjGetLocalTagMatrix

};

std::array<CGameMemoryTweak<unsigned long>, 2> cod4x_tweaks;

static void write_bytes(auto dst, const std::string& bytes)
{
    if (!Engine::Tools::write_bytes(dst, bytes)) {
        std::cout << std::format("write_bytes failed: {:x}\n", dst);
    }
}

void CG_MemoryTweaks()
{
    if (const auto p = CoD4X::Get()) {
        cod4x_tweaks[0] =
            //this thing just tries to crash your game (checks for installed hooks)
            CGameMemoryTweak{ .address = p + 0x43580, .tweaked = "\xC3", .original = "\x55" };

        cod4x_tweaks[1] =
            //put a return to the beginning of CG_DObjGetLocalTagMatrix (crashes your game if you call this yourself)
            CGameMemoryTweak{ .address = p + 0x431B0, .tweaked = "\xC3", .original = "\x57" };
    }

    for (const auto& tweak : tweaks) {
        write_bytes(tweak.address, tweak.tweaked);
    }
    if (CoD4X::Get())
        for (const auto& tweak : cod4x_tweaks)
            write_bytes(tweak.address, tweak.tweaked);
}

void CG_MemoryTweakCleanup()
{
    for (const auto& tweak : tweaks)
        write_bytes(tweak.address, tweak.original);

    if (CoD4X::Get())
        for (const auto& tweak : cod4x_tweaks)
            write_bytes(tweak.address, tweak.original);
}

