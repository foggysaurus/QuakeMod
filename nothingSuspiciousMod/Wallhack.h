#pragma once
#include <vector>

#include "memory.h"
#include "Actions.h"

///////////////////////////////////////////////////////////////
// class Wh16284 - wallhack which spams values into memory to invoke fake sense 3
class Wh16384 
{
public:
    std::vector<int> adds16384 =
    {
        0x63DF40,
        0x644E04,
        0x645A6C,
        0x6466D4,
        0x64733C,
        0x647FA4,
        0x648C0C,
        0x649874,
        0x64A4DC,
        0x64B144,
    };

    std::vector<int> adds_level3 =
    {
        0x63DFD8,
        0x644E9C,
        0x645B04,
        0x64676C,
        0x6473D4,
        0x64803C,
        0x648CA4,
        0x64990C,
        0x64A574,
        0x64B1DC,
        0x64BE44,
        0x64CAAC,
        0x64D714,
    };
public:
    void Wh(uintptr_t mbiiBase, bool isAfk, int level);
private:
    int mbiiBase = 0;

};

