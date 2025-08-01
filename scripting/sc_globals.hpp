#pragma once

namespace Varjus {
	struct State;
	enum Success : signed char;
}

[[nodiscard]] Varjus::Success SC_AddGlobalObject(Varjus::State& state);