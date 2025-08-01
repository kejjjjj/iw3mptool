#include "cg/cg_local.hpp"
#include "cmd/cmd.hpp"
#include "com/com_channel.hpp"
#include "dvar/dvar.hpp"
#include "scripting/sc_main.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/varjus_api.hpp"

#include "sc_self.hpp"
#include "sc_level.hpp"
#include "sc_globals.hpp"
#include "cm/sc_world.hpp"

#include <optional>
#include <thread>

VARJUS_DEFINE_CALLBACK(PrintBold, ctx, args)
{
	VarjusString s;

	for (auto& arg : args)
		s += arg->ValueAsEscapedString();

	Com_Printf(CON_CHANNEL_BOLDGAME, "%s\n", s.c_str());

	return IValue::Construct(ctx->m_pRuntime);
}
VARJUS_DEFINE_CALLBACK(PrintLn, ctx, args)
{
	VarjusString s;

	for (auto& arg : args)
		s += arg->ValueAsEscapedString();

	Com_Printf("%s\n", s.c_str());

	return IValue::Construct(ctx->m_pRuntime);
}
VARJUS_DEFINE_CALLBACK(SendCommandCallback, ctx, args)
{
	if (args[0]->Type() != Varjus::t_string)
		throw Varjus::CRuntimeError(ctx->m_pRuntime, "expected a string");

	CBuf_ExecuteBuffer(args[0]->ToString().c_str());
	return IValue::Construct(ctx->m_pRuntime);
}
VARJUS_DEFINE_CALLBACK(SleepFunc, ctx, args)
{
	if (!args[0]->IsIntegral())
		throw Varjus::CRuntimeError(ctx->m_pRuntime, "expected an integral value");

	std::this_thread::sleep_for(std::chrono::milliseconds(args[0]->ToInt()));
	return IValue::Construct(ctx->m_pRuntime);
}
VARJUS_DEFINE_CALLBACK(GetDvarInt, ctx, args)
{
	if (args[0]->Type() != Varjus::t_string)
		throw Varjus::CRuntimeError(ctx->m_pRuntime, "expected a string");
	
	const auto& str = args[0]->ToString();
	const auto dvar = Dvar_FindMalleableVar(str.c_str());
	if(!dvar)
		return IValue::Construct(ctx->m_pRuntime);

	if(dvar->type == dvar_type::boolean)
		return CIntValue::Construct(ctx->m_pRuntime, static_cast<VarjusInt>(dvar->current.enabled));

	return CIntValue::Construct(ctx->m_pRuntime, dvar->current.integer);
}
VARJUS_DEFINE_CALLBACK(GetDvarFloat, ctx, args)
{
	if (args[0]->Type() != Varjus::t_string)
		throw Varjus::CRuntimeError(ctx->m_pRuntime, "expected a string");

	const auto& str = args[0]->ToString();
	const auto dvar = Dvar_FindMalleableVar(str.c_str());
	if (!dvar)
		return IValue::Construct(ctx->m_pRuntime);

	return CDoubleValue::Construct(ctx->m_pRuntime, dvar->current.value);
}
VARJUS_DEFINE_CALLBACK(GetDvarString, ctx, args)
{
	if (args[0]->Type() != Varjus::t_string)
		throw Varjus::CRuntimeError(ctx->m_pRuntime, "expected a string");

	const auto& str = args[0]->ToString();
	const auto dvar = Dvar_FindMalleableVar(str.c_str());
	if (!dvar)
		return IValue::Construct(ctx->m_pRuntime);

	return CStringValue::Construct(ctx->m_pRuntime, dvar->current.string);
}

Varjus::Success SC_AddMainFunctions(Varjus::State& state)
{
	if (!state.AddNewCallback("printlnbold", PrintBold, Varjus::UNCHECKED_PARAMETER_COUNT))
		return Varjus::failure;

	if (!state.AddNewCallback("println", PrintLn, Varjus::UNCHECKED_PARAMETER_COUNT))
		return Varjus::failure;

	if (!state.AddNewCallback("send_command", SendCommandCallback, Varjus::UNCHECKED_PARAMETER_COUNT))
		return Varjus::failure;

	if (!state.AddNewCallback("sleep", SleepFunc, 1))
		return Varjus::failure;

	if (!state.AddNewCallback("get_dvar_int", GetDvarInt, 1))
		return Varjus::failure;

	if (!state.AddNewCallback("get_dvar_float", GetDvarFloat, 1))
		return Varjus::failure;

	if (!state.AddNewCallback("get_dvar_string", GetDvarString, 1))
		return Varjus::failure;

	return Varjus::success;
}
Varjus::Success SC_AddMainObjects(Varjus::State& state)
{
	using func = Varjus::Success(*)(Varjus::State&);

	const func arr[] = { 
		SC_AddPlayerObject, 
		SC_AddGlobalObject, 
		SC_AddLevelObject, 
		SC_AddWorldObjects 
	};

	for (const auto f : arr) {
		if (!f(state))
			return Varjus::failure;
	}

	return Varjus::success;
}