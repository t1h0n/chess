#pragma once
#include <cstdint>

inline constexpr std::size_t operator""_sz(unsigned long long int num)
{
    return num;
}