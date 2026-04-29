#pragma once
#include <type_traits>
#include <string>
#include <Windows.h>
#include <random>

template<typename T>
concept MemoryAddress_t = std::is_integral_v<T> || std::is_pointer_v<T>;

namespace Engine
{
	template<typename Return, MemoryAddress_t Mem, typename ... Args>
	constexpr Return call(const Mem address, Args... args)
	{
		return (reinterpret_cast<Return(*)(Args...)>(address))(std::forward<Args>(args)...);
	}
}

using namespace std::string_literals;

namespace Engine::Tools
{

	template<MemoryAddress_t Mem>
	[[maybe_unused]] bool write_bytes(Mem dst, const std::string& bytes) {
		DWORD oldProtect = {};
		const auto size = bytes.length();
		if (!VirtualProtect(reinterpret_cast<LPVOID>(dst), size, PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;
		
		memcpy_s(reinterpret_cast<LPVOID>(dst), size, bytes.c_str(), size);

		if (!VirtualProtect(reinterpret_cast<LPVOID>(dst), size, oldProtect, &oldProtect))
			return false;

		return true;
	}

	template<MemoryAddress_t Mem>
	[[maybe_unused]] bool nop(Mem dst) {
		return write_bytes(dst, "\x90\x90\x90\x90\x90"s);
	}
	
	template<typename T>
	concept IsArithmetic = std::is_arithmetic_v<T>;

	inline std::random_device rd;
	inline std::mt19937 mt(rd());

	template<IsArithmetic T>
	inline T random(T range) { //0 -> HI
		std::uniform_real_distribution num{ static_cast<float>(0), static_cast<float>(range) };
		return static_cast<T>(num(mt));
	}
	template<IsArithmetic T>
	inline T random(T min, T range) { //LO -> HI
		std::uniform_real_distribution num{ static_cast<float>(min), static_cast<float>(range) };
		return static_cast<T>(num(mt));
	}

}
