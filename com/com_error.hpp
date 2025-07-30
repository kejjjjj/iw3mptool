#pragma once

#include <string>
#include <format>
#include <utility>

template<typename ... Args>
inline void Com_FatalError(const std::string& format, const Args&&... args)
{
	std::string fmt = std::format(format, std::forward<Args&&>(args)...);
	*(bool*)0xD5EC496 = true;
	strcpy_s((char*)0x1475ED0, fmt.size(), fmt.c_str());
	((void(*)())0x4FD030)();
}

inline void Com_FatalError(const std::string& format)
{
	*(bool*)0xD5EC496 = true;
	strcpy_s((char*)0x1475ED0, format.size(), format.c_str());
	((void(*)())0x4FD030)();

}
