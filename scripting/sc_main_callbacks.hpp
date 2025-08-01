
#include <vector>

namespace Varjus
{
	struct State;
	enum Success : signed char;
}

[[nodiscard]] Varjus::Success SC_AddMainFunctions(Varjus::State& state);
[[nodiscard]] Varjus::Success SC_AddMainObjects(Varjus::State& state);
