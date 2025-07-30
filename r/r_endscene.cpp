#include "cg/cg_cleanup.hpp"
#include "cg/cg_init.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cod4x/cod4x.hpp"
#include "com/com_error.hpp"
#include "r_drawactive.hpp"
#include "r_endscene.hpp"
#include "utils/hook.hpp"
#include "cm/cm_renderer.hpp"

long __stdcall R_EndScene(IDirect3DDevice9* device)
{
	auto h = hooktable::find<long, IDirect3DDevice9*>(HOOK_PREFIX(__func__));
	if (!h) {
		return Com_FatalError("WndProc -> hook == nullptr"), 1;
	}

	return h->call(device, cc_stdcall);
}

void RB_DrawDebug(GfxViewParms* viewParms)
{

	hooktable::find<void, GfxViewParms*>(HOOK_PREFIX(__func__))->call(viewParms);

	CM_ShowCollision(viewParms);
}


void R_RecoverLostDevice()
{
	hooktable::find<void>(HOOK_PREFIX(__func__))->call();

#ifdef IMGUI_API
	if (ImGui::GetCurrentContext()) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}
#endif

	return;

}
void CL_ShutdownRenderer()
{
#ifdef IMGUI_API
	if (ImGui::GetCurrentContext()) {
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		ImGui::SetCurrentContext(nullptr);
	}
#endif

	return hooktable::find<void>(HOOK_PREFIX(__func__))->call();
}
int __cdecl R_CreateDeviceInternal(HWND__* hwnd, int BehaviorFlags, _D3DPRESENT_PARAMETERS_* d3dpp)
{
	auto r = hooktable::find<int, HWND__*, int, _D3DPRESENT_PARAMETERS_*>(HOOK_PREFIX(__func__))->call(hwnd, BehaviorFlags, d3dpp);

#ifdef IMGUI_API
	if (r >= 0)
		CStaticMainGui::Owner->Init(dx->device, FindWindow(NULL, CoD4X::Get() ? "Call of Duty 4 X" : "Call of Duty 4"));
#endif

	return r;
}