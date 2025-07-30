#include "cm_debug.hpp"
#include "cm_brush.hpp"
#include "cmd/cmd.hpp"
#include "dvar/dvar.hpp"
#include "global/defs.hpp"
#include "r/r_drawtools.hpp"

#include <format>

volatile int CGDebugData::tessVerts{};
volatile int CGDebugData::tessIndices{};
GfxPointVertex CGDebugData::g_debugPolyVerts[2725];

void CM_LoadDvars()
{
	Cmd_AddCommand("cm_showCollisionFilter", CM_ShowCollisionFilter);

	static const char* showCollisionNames[] = { "Disabled", "Brushes", "Terrain", "Both" };
	static const char* poly_types[] = { "Edges", "Polygons", "Both" };

	Dvar_RegisterEnum("cm_showCollision", showCollisionNames, 4, 3, dvar_flags::saved, 
		"Select whether to show the terrain, brush or all collision surface groups" );

	Dvar_RegisterEnum("cm_showCollisionPolyType", poly_types, 3, 2, dvar_flags::saved, 
		"Select whether to display the collision surfaces as wireframe or poly interiors");

	Dvar_RegisterFloat("cm_showCollisionDist", 2000.f, 0.f, FLT_MAX, dvar_flags::saved, "Maximum distance to show collision surfaces");
	Dvar_RegisterFloat("cm_showCollisionPolyAlpha", 0.1f, 0.f, 1.f, dvar_flags::saved, "Collision transparency");

	Dvar_RegisterBool("cm_ignoreNonColliding", dvar_flags::saved, true, "Don't display surfaces which don't have collisions");
	Dvar_RegisterBool("cm_depthTest", dvar_flags::saved, true, "Select whether to use depth test in surface display");
	
	Dvar_RegisterBool("cm_onlyBounces", dvar_flags::saved, false, "Only display surfaces which can be bounced");
	Dvar_RegisterBool("cm_onlyElevators", dvar_flags::saved, false, "Only display surfaces which can be elevated");
	Dvar_RegisterBool("cm_disableTriggers", dvar_flags::none, false, "Triggers will not have any effect");
	Dvar_RegisterBool("cm_showEntityConnections", dvar_flags::none, false, "Draw lines between connected entities");

}

void CM_DrawDebug(float& y)
{
	char buff[256];
	sprintf_s(buff,
		"verts:   %i\n"
		"indices: %i\n",
		CGDebugData::tessVerts, CGDebugData::tessIndices);

	R_DrawTextWithEffects(buff, "fonts/bigdevFont", 0, y, 0.15f, 0.2f, 0, vec4_t{1,0.753f,0.796f,0.7f}, 3, vec4_t{1,0,0,0});

	y += 40.f;
}