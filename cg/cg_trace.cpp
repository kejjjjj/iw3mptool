#include "cg_trace.hpp"
#include "cg/cg_local.hpp"
#include "cg_offsets.hpp"

trace_t CG_TracePoint(const fvec3& mins, const fvec3& maxs, const fvec3& start, const fvec3& end, int mask)
{
	trace_t trace;
	CG_TracePoint((const float*)&maxs, &trace, (const float*)&start, (const float*)&mins, (const float*)&end, cgs->clientNum, mask, 1, 1);
	return trace;
}
void __cdecl CG_TracePoint(const vec3_t maxs, trace_t* trace, const vec3_t start, const vec3_t mins, const vec3_t end, int entityNum, int contentMask, int unknw0, int traceStaticModels)
{

	_asm
	{
		mov eax, [contentMask];
		mov ecx, [entityNum];
		mov edx, [end];
		push edi;
		mov edi, [trace];
		push traceStaticModels;
		push unknw0;
		push eax;
		mov eax, [mins];
		push ecx;
		mov ecx, [start];
		push edx;
		push eax;
		mov eax, [maxs];
		push ecx;
		mov esi, 0x459EF0;
		call esi;
		add esp, 0x1C + 4;
	}
}
trace_t PM_PlayerTrace(pmove_t* pm, const fvec3& mins, const fvec3& maxs, const fvec3& start, const fvec3& end, int mask)
{
	constexpr static auto func = 0x40E160;
	trace_t trace{};
	const auto clientNum = pm->ps->clientNum;
	__asm
	{
		mov esi, pm;
		push mask;
		push clientNum;
		push end;
		push maxs;
		push mins;
		push start;
		lea eax, trace;
		push eax;
		call func;
		add esp, 24;

	}

	return trace;
}
