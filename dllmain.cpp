#include <Windows.h>
#include <thread>
#include <chrono>

#include "cg/cg_init.hpp"

using namespace std::chrono_literals;

#define USE_CONSOLE 0

constexpr auto MODULE_NAME = "iw3mp.exe";


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
{
    std::thread thread;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        thread = std::thread([&hModule, lpReserved]() {
            if (!GetModuleHandle(MODULE_NAME)) {
                return 0;
            }

#if(USE_CONSOLE)
            FILE* _con = 0;
            AllocConsole();
            freopen_s(&_con, "CONOUT$", "w", stdout);
#endif
            CG_Init();

#if(USE_CONSOLE)
            if (_con)
                fclose(_con);
#endif

            while (!!true) {
                std::this_thread::sleep_for(1s);
            }


            return 1;

            });
        thread.detach();


        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


