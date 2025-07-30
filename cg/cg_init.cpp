
#include "cg_local.hpp"
#include "cg_offsets.hpp"
#include "cg_memory.hpp"
#include "cg_hooks.hpp"
#include "cm/cm_debug.hpp"
#include "cod4x/cod4x.hpp"
#include "utils/hook.hpp"
#include "sys/sys_thread.hpp"
#include "dvar/dvar.hpp"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
void CG_Init()
{
	while (!dx || !dx->device) {
		std::this_thread::sleep_for(200ms);
	}

	srand(static_cast<unsigned>(time(0)));

	Sys_SuspendAllThreads();
	std::this_thread::sleep_for(300ms);

	CoD4X::Initialize();

	Dvar_RegisterBool("sec_executeCommands", dvar_flags::saved, true,
		"When disabled, the client will ignore ALL commands sent by the server");


	CM_LoadDvars();

	CG_CreateHooks();
	CG_MemoryTweaks();
	



	Sys_ResumeAllThreads();
}