#include "sc_main.hpp"

#include "cg/cg_offsets.hpp"
#include "cg/cg_local.hpp"
#include "cg/cg_angles.hpp"
#include "g/g_entities.hpp"
#include "cm/cm_entity.hpp"
#include "scr/scr_functions.hpp"

static CArrayValue* GetAllPlayers(Varjus::CProgramRuntime* const rt)
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

static CArrayValue* GetAllEntities(Varjus::CProgramRuntime* const rt)
{
	IValues arr;
	arr.reserve(CGentities::Size());

	const auto GetFields = [&rt](GentityPtr_t& gent) {
		CObjectValue* obj = CObjectValue::Construct(rt, {});

		for (const auto& [k, v] : gent->GetFields()) {
			obj->AddAttribute(CStringValue::Construct(rt, k), CStringValue::Construct(rt, v));
		}

		return obj;
	};

	CGentities::ForEach([&](GentityPtr_t& gent) {

		auto owner = gent->GetOwner();

		if (!owner)
			return;

		CObjectValue* obj = ToKVObject(rt,
			std::make_pair("classname"s, CStringValue::Construct(rt, Scr_GetString(owner->classname))),
			std::make_pair("origin"s, ToVec3FromObject(rt, owner->r.currentOrigin)),
			std::make_pair("angles"s, ToVec3FromObject(rt, owner->r.currentAngles)),
			std::make_pair("mins"s, ToVec3FromObject(rt, owner->r.absmin)),
			std::make_pair("maxs"s, ToVec3FromObject(rt, owner->r.absmax)),
			std::make_pair("fields"s, GetFields(gent))
		);

		arr.push_back(obj);
	});

	return CArrayValue::Construct(rt, std::move(arr));

}

VARJUS_DEFINE_PROPERTY(GetEntities, ctx, _this) {
	return GetAllEntities(ctx->m_pRuntime);
}

VARJUS_DEFINE_STATIC_OBJECT(LevelObject, receiver)
{
	receiver.AddProperty(VSL("players"), GetPlayers);
	receiver.AddProperty(VSL("entities"), GetEntities);

}

Varjus::Success SC_AddLevelObject(Varjus::State& state)
{
	if (!state.AddNewStaticObject(VSL("level"), LevelObject))
		return Varjus::failure;

	return Varjus::success;
}