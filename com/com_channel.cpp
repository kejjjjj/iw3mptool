#include "com_channel.hpp"
#include "utils/engine.hpp"
#include "cg/cg_local.hpp"
#include "cod4x/cod4x.hpp"

void Com_Printf($B87C0110D100A68234FECCEB9075A41E channel, const char* msg, ...)
{
	char v2[4096];
	va_list va; 

	va_start(va, msg);
	_vsnprintf_s(v2, 0x1000u, msg, va);
	v2[4095] = 0;

	return Engine::call<void>(0x4FCBC0, channel, v2, 0);

}
void Com_Printf(const char* msg, ...)
{
	char v2[4096];
	va_list va;

	va_start(va, msg);
	_vsnprintf_s(v2, 0x1000u, msg, va);
	v2[4095] = 0;

	return Engine::call<void>(0x4FCBC0, CON_CHANNEL_OBITUARY, v2, 0);
}

void Com_Error(errorParm_t category, const char* msg, ...)
{
	char v2[4096];
	va_list va;

	va_start(va, msg);
	_vsnprintf_s(v2, 0x1000u, msg, va);
	v2[4095] = 0;

	return Engine::call<void>(0x4FD330, category, v2);

}

void iPrintLn(const char* msg)
{
	__asm
	{

		push msg
		push 0
		mov esi, 0x43dda0
		call esi
		add esp, 0x8
	}
}
void iPrintLnBold(const char* msg, ...)
{
	char v2[4096];
	va_list va;

	va_start(va, msg);
	_vsnprintf_s(v2, 0x1000u, msg, va);
	v2[4095] = 0;

	__asm
	{

		push dword ptr v2
		push 0
		mov esi, 0x43de00
		call esi
		add esp, 0x8
	}
}