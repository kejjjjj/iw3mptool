#include "cl_main.hpp"
#include "utils/hook.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cm/cm_typedefs.hpp"
#include "cm/cm_entity.hpp"

void CL_Disconnect(int clientNum)
{
	if (clientUI->connectionState != CA_DISCONNECTED) { //gets called in the loading screen in 1.7
		CClipMap::ClearThreadSafe();
		CGentities::ClearThreadSafe();
		CStaticEntityFields::m_oGentityFields.clear();

	}

	hooktable::find<void, int>(HOOK_PREFIX(__func__))->call(clientNum);

}
