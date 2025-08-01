#include "sc_self.hpp"
#include "varjus_api/varjus_api.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"

#include "cg/cg_offsets.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_angles.hpp"
#include "utils/typedefs.hpp"


template<typename ... Args>
CObjectValue* ToKVObject(Varjus::CProgramRuntime* const rt, Args&&... args) {
	return CObjectValue::Construct(rt, { { CStringValue::Construct(rt, std::get<0>(args)), std::get<1>(args) }... });
}

inline auto ToVec3FromObject(Varjus::CProgramRuntime* const rt, const fvec3& vec)
{
	IValues values = { CDoubleValue::Construct(rt, vec.x), CDoubleValue::Construct(rt, vec.y), CDoubleValue::Construct(rt, vec.z) };

	return CArrayValue::Construct(rt, std::move(values));
	//return ToKVObject(rt,
	//	std::make_pair(VSL("x"), CDoubleValue::Construct(rt, vec.x)),
	//	std::make_pair(VSL("y"), CDoubleValue::Construct(rt, vec.y)),
	//	std::make_pair(VSL("z"), CDoubleValue::Construct(rt, vec.z))
	//);
}


VARJUS_DEFINE_PROPERTY(GetPlayerAngles, ctx, _this)
{
	const auto cmd = &clients->cmds[clients->cmdNumber & 0x7F];
	const auto angles = CG_AnglesFromCmd(cmd);

	return ToVec3FromObject(ctx->m_pRuntime, angles);
}
VARJUS_DEFINE_PROPERTY(GetPlayerOrigin, ctx, _this)
{
	return ToVec3FromObject(ctx->m_pRuntime, clients->cgameOrigin);
}
VARJUS_DEFINE_PROPERTY(GetPlayerVelocity, ctx, _this)
{
	return ToVec3FromObject(ctx->m_pRuntime, clients->cgameVelocity);
}
VARJUS_DEFINE_PROPERTY(GetPlayerViewOrigin, ctx, _this)
{
	return ToVec3FromObject(ctx->m_pRuntime, rg->viewOrg);
}
VARJUS_DEFINE_PROPERTY(GetPlayerViewHeight, ctx, _this)
{
	return CDoubleValue::Construct(ctx->m_pRuntime, cgs->predictedPlayerState.viewHeightCurrent);
}
VARJUS_DEFINE_PROPERTY(GetPlayerPMFlags, ctx, _this)
{
	return CIntValue::Construct(ctx->m_pRuntime, cgs->predictedPlayerState.pm_flags);
}
VARJUS_DEFINE_PROPERTY(GetPlayerSpeed, ctx, _this)
{
	return CIntValue::Construct(ctx->m_pRuntime, cgs->predictedPlayerState.speed);
}
VARJUS_DEFINE_PROPERTY(GetPlayerGravity, ctx, _this)
{
	return CIntValue::Construct(ctx->m_pRuntime, cgs->predictedPlayerState.gravity);
}
VARJUS_DEFINE_PROPERTY(GetPlayerPMTime, ctx, _this)
{
	return CIntValue::Construct(ctx->m_pRuntime, cgs->predictedPlayerState.pm_time);
}
VARJUS_DEFINE_PROPERTY(GetPlayerClientNum, ctx, _this) {
	return CIntValue::Construct(ctx->m_pRuntime, cgs->predictedPlayerState.clientNum);
}
VARJUS_DEFINE_METHOD(SetPlayerAngles, ctx, _this, args)
{
	for (auto i = std::size_t{ 0 }; i < args.size(); i++) {
		if (!args[i]->IsArithmetic())
			throw Varjus::CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("arg[{}] must be arithmetic!"), i));
	}
	const auto cmd = &clients->cmds[clients->cmdNumber & 0x7F];
	const fvec3 target = {
		static_cast<float>(args[0]->ToDouble()),
		static_cast<float>(args[1]->ToDouble()),
		static_cast<float>(args[2]->ToDouble())
	};

	CL_SetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, target);
	return IValue::Construct(ctx->m_pRuntime);
}
VARJUS_DEFINE_METHOD(PlayerLookAt, ctx, _this, args)
{
	for (auto i = std::size_t{ 0 }; i < args.size(); i++) {
		if (!args[i]->IsArithmetic())
			throw Varjus::CRuntimeError(ctx->m_pRuntime, Varjus::fmt::format(VSL("arg[{}] must be arithmetic!"), i));
	}
	const auto cmd = &clients->cmds[clients->cmdNumber & 0x7F];
	const fvec3 target = { 
		static_cast<float>(args[0]->ToDouble()), 
		static_cast<float>(args[1]->ToDouble()),
		static_cast<float>(args[2]->ToDouble()) 
	};

	const auto final = (target - rg->viewOrg).toangles();

	CL_SetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, final);

	return IValue::Construct(ctx->m_pRuntime);
}
VARJUS_DEFINE_METHOD(PlayerAnglesToForward, ctx, _this, args)
{
	const auto cmd = &clients->cmds[clients->cmdNumber & 0x7F];
	const auto angles = CG_AnglesFromCmd(cmd);

	return ToVec3FromObject(ctx->m_pRuntime, angles.toforward());
}

VARJUS_DEFINE_STATIC_OBJECT(PlayerObject, receiver)
{
	receiver.AddProperty(VSL("angles"), GetPlayerAngles);
	receiver.AddProperty(VSL("origin"), GetPlayerOrigin);
	receiver.AddProperty(VSL("velocity"), GetPlayerVelocity);
	receiver.AddProperty(VSL("view_origin"), GetPlayerViewOrigin);
	receiver.AddProperty(VSL("view_height"), GetPlayerViewHeight);
	receiver.AddProperty(VSL("pm_flags"), GetPlayerPMFlags);
	receiver.AddProperty(VSL("pm_time"), GetPlayerPMTime);
	receiver.AddProperty(VSL("gravity"), GetPlayerGravity);
	receiver.AddProperty(VSL("speed"), GetPlayerSpeed);
	receiver.AddProperty(VSL("id"), GetPlayerClientNum);

	receiver.AddMethod(VSL("set_angles"), SetPlayerAngles, 3);
	receiver.AddMethod(VSL("look_at"), PlayerLookAt, 3);
	receiver.AddMethod(VSL("angles_to_forward"), PlayerAnglesToForward, 0);
}

Varjus::Success SC_AddPlayerObject(Varjus::State& state)
{
	if (!state.AddNewStaticObject(VSL("self"), PlayerObject))
		return Varjus::failure;

	return Varjus::success;
}