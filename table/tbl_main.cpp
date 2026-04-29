#include "table/tbl_main.hpp"
#include "cg/cg_local.hpp"
#include "utils/engine.hpp"

const char* StringTable_Lookup(const StringTable* table, int comparisonColumn, const char* value, int valueColumn)
{
	constexpr auto fnc = 0x569B30;
	__asm {
		push value;
		push comparisonColumn;
		mov esi, table;
		mov edi, valueColumn;
		call fnc;
		add esp, 8;
	}
}
#define MU [[maybe_unused]]
const char* TableLookup(const char* filename, MU int comparisonColumn, MU const char* searchValue, MU int returnValueColumnNum) {
	auto header = Engine::call<XAssetHeader>(0x489570, XAssetType::ASSET_TYPE_STRINGTABLE, filename);
	auto stringTable = reinterpret_cast<StringTable*>(header.data);
	return StringTable_Lookup(stringTable, comparisonColumn, searchValue, returnValueColumnNum);
}