#include "sys_thread.hpp"
#include "cg/cg_offsets.hpp"
#include <Windows.h>

void Sys_SuspendAllThreads()
{
	for (auto i = 0u; i < NUM_GAME_THREADS; i++) {
		SuspendThread((HANDLE)threadHandles[i]);
	}
}
void Sys_ResumeAllThreads()
{
	for (auto i = 0u; i < NUM_GAME_THREADS; i++) {
		ResumeThread((HANDLE)threadHandles[i]);
	}
}

