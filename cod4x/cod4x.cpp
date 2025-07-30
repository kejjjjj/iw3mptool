#include "cg/cg_local.hpp"
#include "cg/cg_offsets.hpp"
#include "cod4x.hpp"
#include "dvar/dvar.hpp"
#include "utils/engine.hpp"

unsigned long CoD4X::m_pModule{};


void CoD4X::Initialize()
{
	if ((m_pModule = reinterpret_cast<DWORD>(GetModuleHandle("cod4x_021.dll"))) == false)
		return;

	UpdateOffsets();

}
bool CoD4X::LegacyProtocol() 
{
	if (!Get())
		return false;

	if (const auto dvar = Dvar_FindMalleableVar("legacyProtocol"))
		return dvar->current.enabled;

	return false;
}
void CoD4X::UpdateOffsets()
{
	BG_WeaponNames = reinterpret_cast<WeaponDef**>(Get() + 0x443DDE0);
}
