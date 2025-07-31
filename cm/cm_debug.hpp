
#pragma once

#include "cg/cg_local.hpp"

void CM_LoadDvars();
void CM_DrawDebug(float& y, const vec4_t color);

struct CGDebugData
{
	static volatile int tessVerts;
	static volatile int tessIndices;
	static GfxPointVertex g_debugPolyVerts[2725];
};
