#pragma once

//from the looks of it cod4x will never update from 21.1
class CoD4X
{
public:

	static void Initialize();

	[[nodiscard]] static auto Get() { return m_pModule; }
	[[nodiscard]] static bool LegacyProtocol();

private:
	static void UpdateOffsets();

	static unsigned long m_pModule;

};
