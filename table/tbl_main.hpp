#pragma once


[[nodiscard]] const char* StringTable_Lookup(const struct StringTable* table, int comparisonColumn, const char* value, int valueColumn);

[[nodiscard]] const char* TableLookup(const char* filename, int searchColumnNum, const char* searchValue, int returnValueColumnNum);
[[nodiscard]] const char* TableLookupIString(const char* filename, int searchColumnNum, const char* searchValue, int returnValueColumnNum);