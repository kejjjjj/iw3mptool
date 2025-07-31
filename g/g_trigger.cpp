#include "g_trigger.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "dvar/dvar.hpp"
#include "utils/hook.hpp"

struct gentity_s* self, other;
void G_Trigger()
{
	__asm {
		mov self, edx;
		mov other, esi;
	}

	if (const auto dvar = Dvar_FindMalleableVar("cm_disableTriggers")) {
		if (dvar->current.enabled)
			return;
	}

	auto jmpptr = hooktable::find<void>(HOOK_PREFIX(__func__))->get_jmp();

	__asm
	{
		mov edx, self;
		mov esi, other;
		call jmpptr;
	}

	return;
}
