#pragma once

#include "varjus_api/varjus_api.hpp"
#include "varjus_api/internal/variables.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "utils/typedefs.hpp"

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <ranges>


namespace Varjus {
	struct State;
	enum Success : signed char;
}

void Varjus_Init();
void Varjus_LoadDvars();
void Varjus_Exec_f();
void Varjus_ExecFrame();

using sc_init_functions_t = std::function<Varjus::Success(Varjus::State&)>;
using sc_init_objects_t = std::function<Varjus::Success(Varjus::State&)>;

class CScriptData
{
public:

	[[nodiscard]] static Varjus::Success SC_AddFunctions(Varjus::State& state);
	[[nodiscard]] static Varjus::Success SC_AddObjects(Varjus::State& state);

	[[nodiscard]] static bool SC_Execute(const std::string& script);
	[[nodiscard]] static bool SC_ExecuteFile(const std::string& script);

	[[nodiscard]] static bool SC_ExecuteAsynchronously(Varjus::State& state, const std::string& script);
	[[nodiscard]] static bool SC_ExecuteFileAsynchronously(Varjus::State& state, const std::string& script);

	static void Queue_SC_InitFunctions(sc_init_functions_t func);
	static void Queue_SC_InitObjects(sc_init_objects_t func);

private:


	static std::vector<sc_init_functions_t> m_SC_InitFunctions;
	static std::vector<sc_init_objects_t> m_SC_InitObjects;
};


template<typename ... Args>
inline CObjectValue* ToKVObject(Varjus::CProgramRuntime* const rt, Args&&... args) {
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

template<std::size_t count = 3>
inline fvec3 IsVecArray(Varjus::CProgramRuntime* const rt, const IValue* v) {
	auto asArray = v->ToArray();

	auto& vars = asArray->Internal()->GetContent().GetVariables();
	if (vars.size() != count)
		throw Varjus::CRuntimeError(rt, VSL("expected 3 elements for the array"));

	fvec3 vec3;
	for (std::size_t i{}; auto& var : vars | std::views::take(count)) {
		if (!var->GetValue()->IsArithmetic())
			throw Varjus::CRuntimeError(rt, VSL("expected an arithmetic type"));

		vec3[i++] = static_cast<float>(var->GetValue()->ToDouble());
	}

	return vec3;
};