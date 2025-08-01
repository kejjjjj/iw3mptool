#pragma once

struct trace_t;
struct pmove_t;

template <typename T>
struct vec3;

using fvec3 = vec3<float>;
using vec3_t = float[3];

trace_t CG_TracePoint(const fvec3& mins, const fvec3& maxs, const fvec3& start, const fvec3& end, int mask);
void __cdecl CG_TracePoint(const vec3_t maxs, trace_t* trace, const vec3_t start, const vec3_t mins, const vec3_t end, int entityNum, int contentMask, int locational, int staticModels);
inline void (*CG_Trace)(trace_t* result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipEntity, int tracemask) = (decltype(CG_Trace))0x45A230;
trace_t PM_PlayerTrace(pmove_t* pm, const fvec3& mins, const fvec3& maxs, const fvec3& start, const fvec3& end, int mask);