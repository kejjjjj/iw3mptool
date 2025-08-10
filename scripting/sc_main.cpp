#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "com/com_channel.hpp"
#include "sc_main.hpp"
#include "varjus_api/varjus_api.hpp"
#include "dvar/dvar.hpp"
#include "cmd/cmd.hpp"
#include "fs/fs_globals.hpp"

#include "sc_main_callbacks.hpp"

#include <optional>
#include <thread>
#include <string>
#include <mutex>
#include <ranges>

void Varjus_Init()
{
	CScriptData::Queue_SC_InitFunctions(SC_AddMainFunctions);
	CScriptData::Queue_SC_InitObjects(SC_AddMainObjects);
}

void Varjus_LoadDvars()
{
	Cmd_AddCommand("varjus_exec", Varjus_Exec_f);

	Dvar_RegisterBool("varjus_async", dvar_flags::saved, true, "run scripts asynchronously");

}

class CScript
{
public:
	static bool m_bExecNow;
	static bool m_bAbort;
	static std::string m_sScriptPath;
	static std::vector<std::string> m_oArgs;
	static std::mutex mtx;
};

bool CScript::m_bExecNow{};
bool CScript::m_bAbort{};
std::string CScript::m_sScriptPath;
std::mutex CScript::mtx;
std::vector<std::string> CScript::m_oArgs;

void Varjus_Exec_f()
{
	CScript::m_oArgs.clear();

	const auto num_args = cmd_args->argc[cmd_args->nesting];

	if (num_args < 2) {
		return Com_Printf(CON_CHANNEL_CONSOLEONLY, "usage: varjus_exec <script_path> <optional args>\n");
	}

	const std::string relative = *(cmd_args->argv[cmd_args->nesting] + 1);
	auto fullPath = __fs::exe_path() + "\\varjus\\" + relative;

	if (__fs::get_extension(relative) != ".var")
		fullPath += ".var";

	if (!__fs::file_exists(fullPath)) {
		return Com_Printf(CON_CHANNEL_CONSOLEONLY, "\"%s\" doesn't exist\n", fullPath.c_str());
	}
	
	std::unique_lock<std::mutex> lock(CScript::mtx);

	if (num_args > 2) {
		for (const auto i : std::views::iota(2, num_args)) {
			CScript::m_oArgs.push_back(*(cmd_args->argv[cmd_args->nesting] + i));
		}
	}

	CScript::m_bExecNow = { true };
	CScript::m_sScriptPath = fullPath;
}

void Varjus_ExecFrame()
{
	static std::unique_ptr<Varjus::State> state;

	if (CScript::m_bExecNow) {
		const auto asyncDvar = Dvar_FindMalleableVar("varjus_async");
		const auto async = asyncDvar && asyncDvar->current.enabled;
		auto status = false;

		if (async) {
			state = std::make_unique<Varjus::State>();
			status = CScriptData::SC_ExecuteFileAsynchronously(*state, CScript::m_sScriptPath);
		} else {
			status = CScriptData::SC_ExecuteFile(CScript::m_sScriptPath);
		}

		std::unique_lock<std::mutex> lock(CScript::mtx);
		CScript::m_bExecNow = { false };
	}

	if (state) {

		if (CScript::m_bAbort) {
			[[maybe_unused]] const auto _ = state->Abort();
			std::unique_lock<std::mutex> lock(CScript::mtx);
			CScript::m_bAbort = { false };
		}

		if (const auto msg = state->GetErrorMessage()) {
			Com_Printf("^1Error: %s\n", msg->c_str());
			state.reset();
		}

		else if (state->HasFinished()) {
			state.reset();
		}
	}
}

VARJUS_DEFINE_ARGS(ParseArgs, ctx, receiver)
{
	for (const auto& arg : CScript::m_oArgs) {
		receiver.push_back(CStringValue::Construct(ctx, arg));
	}
}

static std::string SC_ErrorTranslator(const std::optional<std::string>& err) {
	return err.value_or("unknown error");
}
static bool SC_ThrowError(Varjus::State& state) {
	const auto err = SC_ErrorTranslator(state.GetErrorMessage());
	return Com_Printf("^1Error: %s\n", err.c_str()), false;
}

template<typename ... Args>
static bool SC_Prepare(Varjus::State& state, Varjus::Success(Varjus::State::* fn)(Args...), Args&&... args)
{

	if (!state.UseStdLibrary()) {
		return SC_ThrowError(state);
	}

	if(!CScriptData::SC_AddFunctions(state))
		return SC_ThrowError(state);

	if (!CScriptData::SC_AddObjects(state))
		return SC_ThrowError(state);

	if (!(state.*fn)(std::forward<Args>(args)...))
		return SC_ThrowError(state);

	return true;
}

bool CScriptData::SC_Execute(const std::string& script)
{
	Varjus::State state;

	if (!SC_Prepare(state, &Varjus::State::LoadScript, script))
		return false;

	if (const auto result = state.ExecuteScript(ParseArgs)) {
		return true;
	}

	return SC_ThrowError(state);
}
bool CScriptData::SC_ExecuteFile(const std::string& path)
{
	Varjus::State state;

	if (!SC_Prepare(state, &Varjus::State::LoadScriptFromFile, path, Varjus::e_utf8))
		return false;

	if (const auto result = state.ExecuteScript(ParseArgs)) {
		return true;
	}

	return SC_ThrowError(state);
}

static void AwaitFunc(Varjus::State& state) {
	[[maybe_unused]] const auto result = state.ExecuteScript(ParseArgs);
}

bool CScriptData::SC_ExecuteAsynchronously(Varjus::State& state, const std::string& script)
{
	if (!SC_Prepare(state, &Varjus::State::LoadScript, script))
		return false;

	std::thread thread(AwaitFunc, std::ref(state));
	thread.detach();
	return true;
}
bool CScriptData::SC_ExecuteFileAsynchronously(Varjus::State& state, const std::string& path)
{
	if (!SC_Prepare(state, &Varjus::State::LoadScriptFromFile, path, Varjus::e_utf8))
		return false;

	std::thread thread(AwaitFunc, std::ref(state));
	thread.detach();
	return true;
}

/***********************************************************************
 > 
***********************************************************************/
Varjus::Success CScriptData::SC_AddFunctions(Varjus::State& state)
{
	for (const auto& cb : m_SC_InitFunctions) {

		if (!cb(state))
			return Varjus::failure;
	}

	return Varjus::success;
}
Varjus::Success CScriptData::SC_AddObjects(Varjus::State& state)
{
	for (const auto& cb : m_SC_InitObjects) {

		if (!cb(state))
			return Varjus::failure;
	}

	return Varjus::success;
}

void CScriptData::Queue_SC_InitFunctions(sc_init_functions_t func) { m_SC_InitFunctions.push_back(func); }
void CScriptData::Queue_SC_InitObjects(sc_init_objects_t func) { m_SC_InitObjects.push_back(func); }

std::vector<sc_init_functions_t> CScriptData::m_SC_InitFunctions;
std::vector<sc_init_objects_t> CScriptData::m_SC_InitObjects;




