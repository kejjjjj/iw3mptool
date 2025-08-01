#include "r_drawactive.hpp"
#include "r_drawtools.hpp"
#include "utils/hook.hpp"

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "dvar/dvar.hpp"
#include "cm/cm_debug.hpp"
#include "cm/cm_entity.hpp"
#include "scripting/sc_main.hpp"

static void CG_DrawCoordinates(float& y, const vec4_t color)
{
	auto ps = &cgs->predictedPlayerState;

	char buff[256];
	sprintf_s(buff,
		"x:     %.6f\n"
		"y:     %.6f\n"
		"z:     %.6f\n"
		"yaw: %.6f", ps->origin[0], ps->origin[1], ps->origin[2], ps->viewangles[1]);
	R_DrawTextWithEffects(buff, "fonts/bigdevFont", 0, y, 0.15f, 0.2f, 0, color, 3, vec4_t{ 1,0,0,0 });

	y += 20.f;
}
static void CG_DrawVelocity(float& y, const vec4_t color)
{
	auto ps = &cgs->predictedPlayerState;

	char buff[256];
	sprintf_s(buff,
		"x:     %.1f\n"
		"y:     %.1f\n"
		"z:     %.1f\n"
		"xy:   %.1f\n",
		std::fabsf(ps->velocity[0]), std::fabsf(ps->velocity[1]), std::fabsf(ps->velocity[2]), fvec2(ps->velocity).mag());

	R_DrawTextWithEffects(buff, "fonts/bigdevFont", 0, y, 0.15f, 0.2f, 0, color, 3, vec4_t{ 1,0,0,0 });

	y += 20.f;
}


void CG_DrawActive()
{
	static dvar_s* cm_entityInfo = Dvar_FindMalleableVar("cm_entityInfo");
	static dvar_s* cm_showCollisionDist = Dvar_FindMalleableVar("cm_showCollisionDist");
	static dvar_s* pm_coordinates = Dvar_FindMalleableVar("pm_coordinates");
	static dvar_s* pm_velocity = Dvar_FindMalleableVar("pm_velocity");
	static dvar_s* developer = Dvar_FindMalleableVar("developer");

	float y = 45.f;

	vec4_t col{ 1,0.753f,0.796f,0.7f };

	if (pm_coordinates && pm_coordinates->current.enabled) {
		CG_DrawCoordinates(y, col);
	}

	if (pm_velocity && pm_velocity->current.enabled) {
		CG_DrawVelocity(y, col);
	}

	if (developer && developer->current.enabled) {
		CM_DrawDebug(y, col);
	}
	if (cm_showCollisionDist && cm_entityInfo && cm_entityInfo->current.integer) {

		CGentities::ForEach([](GentityPtr_t& ptr) {
			ptr->CG_Render2D(cm_showCollisionDist->current.value, static_cast<entity_info_type>(cm_entityInfo->current.integer));
		});

	}

	Varjus_ExecFrame();

	return hooktable::find<void>(HOOK_PREFIX(__func__))->call();

}