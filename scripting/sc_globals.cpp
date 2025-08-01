#include "varjus_api/varjus_api.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"

#include "cg/cg_offsets.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_angles.hpp"
#include "utils/typedefs.hpp"
#include "fs/fs_globals.hpp"

VARJUS_DEFINE_PROPERTY(GameDir, ctx, _this) {

	return CStringValue::Construct(ctx->m_pRuntime, __fs::exe_path());
}

VARJUS_DEFINE_STATIC_OBJECT(GlobalObject, receiver)
{
	receiver.AddProperty(VSL("game_directory"), GameDir);
}

Varjus::Success SC_AddGlobalObject(Varjus::State& state)
{
	if (!state.AddNewStaticObject(VSL("global"), GlobalObject))
		return Varjus::failure;

	return Varjus::success;
}