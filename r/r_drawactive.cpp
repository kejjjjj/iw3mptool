#include "r_drawactive.hpp"
#include "r_drawtools.hpp"
#include "utils/hook.hpp"

#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "dvar/dvar.hpp"
#include "cm/cm_debug.hpp"

void CG_DrawActive()
{
	float y = 40.f;

	if (const auto dev = Dvar_FindMalleableVar("developer")) {
		if (dev->current.enabled) {
			CM_DrawDebug(y);
		}

	}

	return hooktable::find<void>(HOOK_PREFIX(__func__))->call();

}