#include "cg_cleanup.hpp"
#include "cg_local.hpp"
#include "cg_offsets.hpp"
#include "cm/cm_brush.hpp"
#include "cod4x/cod4x.hpp"
#include "dvar/dvar.hpp"
#include "r/r_drawactive.hpp"
#include "r/r_endscene.hpp"
#include "utils/hook.hpp"
#include "g/g_trigger.hpp"
#include "menu/menu_parse.hpp"
#include "cl/cl_main.hpp"

static void CG_CreateHooksInternal();
void CG_CreateHooks()
{
	hooktable::initialize();
	CG_CreateHooksInternal();
}

void CG_CreateHooksInternal()
{
	hooktable::preserver<void>(HOOK_PREFIX("CG_DrawActive"), CoD4X::Get() ? CoD4X::Get() + 0x5464 : 0x42F7F0, CG_DrawActive);
	hooktable::preserver<void, GfxViewParms*>(HOOK_PREFIX("RB_DrawDebug"), 0x658860, RB_DrawDebug);
	hooktable::preserver<void, int>(HOOK_PREFIX("CL_Disconnect"), 0x4696B0, CL_Disconnect);


	hooktable::preserver<void>(HOOK_PREFIX("CG_CleanupHook"), 0x477210, CG_CleanupHook);

	hooktable::overwriter<void>(HOOK_PREFIX("__asm_adjacency_winding"), 0x57D86C, __brush::__asm_adjacency_winding);

	hooktable::overwriter<void>(HOOK_PREFIX("G_Trigger"), 0x4E1130, G_Trigger);

	hooktable::preserver<void>(HOOK_PREFIX("CG_SetClientDvarFromServerASM"), 0x44BD90, CG_SetClientDvarFromServerASM);
	hooktable::preserver<void>(HOOK_PREFIX("Script_AddTextWrapper"), 0x54D5D0, Script_AddTextWrapper);
	hooktable::preserver<void>(HOOK_PREFIX("Script_ExecHandler"), 0x54D570, Script_ExecHandler);
	hooktable::preserver<void>(HOOK_PREFIX("Script_ConditionalExecHandler"), 0x54D620, Script_ConditionalExecHandler);

	//if (dx && dx->device)
	//	hooktable::preserver<long, IDirect3DDevice9*>(HOOK_PREFIX("R_EndScene"),
	//		reinterpret_cast<size_t>((*reinterpret_cast<PVOID**>(dx->device))[42]), R_EndScene);

}
