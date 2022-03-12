#pragma once
#include <cstdint>

inline constexpr std::size_t operator""_sz(unsigned long long int num)
{
    return num;
}

inline constexpr std::uint8_t operator""_u8(unsigned long long int num)
{
    return num;
}

inline constexpr std::uint16_t operator""_u16(unsigned long long int num)
{
    return num;
}

inline constexpr std::uint32_t operator""_u32(unsigned long long int num)
{
    return num;
}