#include "sc_main.hpp"

#include "cg/cg_offsets.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_angles.hpp"
#include "g/g_entities.hpp"

CArrayValue* GetAllPlayers(Varjus::CProgramRuntime* const rt)
{
	const auto ents = CG_GetAllEntities(cgs->snap);
	
	IValues arr;
	arr.reserve(ents.size());

	for (const auto& ent : ents) {
		
		CObjectValue* obj = ToKVObject(rt, 
			std::make_pair("name"s, CStringValue::Construct(rt, ent.GetName())),
			std::make_pair("id"s, CIntValue::Construct(rt, ent.GetID())),
			std::make_pair("weapon"s, CIntValue::Construct(rt, ent.GetWeapon())),
			std::make_pair("alive"s, CBooleanValue::Construct(rt, ent.IsAlive())),
			std::make_pair("enemy"s, CBooleanValue::Construct(rt, ent.IsEnemy())),
			std::make_pair("angles"s, ToVec3FromObject(rt, ent.GetAngles())),
			std::make_pair("origin"s, ToVec3FromObject(rt, ent.GetOrigin()))
		);

		arr.push_back(obj);
	}

	return CArrayValue::Construct(rt, std::move(arr));
}

VARJUS_DEFINE_PROPERTY(GetPlayers, ctx, _this)
{
	return GetAllPlayers(ctx->m_pRuntime);
}

VARJUS_DEFINE_STATIC_OBJECT(LevelObject, receiver)
{
	receiver.AddProperty(VSL("players"), GetPlayers);
}

Varjus::Success SC_AddLevelObject(Varjus::State& state)
{
	if (!state.AddNewStaticObject(VSL("level"), LevelObject))
		return Varjus::failure;

	return Varjus::success;
}