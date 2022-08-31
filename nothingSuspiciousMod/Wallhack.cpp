#include "pch.h"
#include "Wallhack.h"


void Wh16384::Wh(uintptr_t mbiiBase,  bool isAfk, int level)
{
    Wh16384 wh16384;
    Actions act;

    if (!isAfk && level != 0)
    {
        for (int i = 0; i < wh16384.adds16384.size(); i++)
        {
            act.WriteToMem(mbiiBase + wh16384.adds16384[i], 16384);
        }
        for (int i = 0; i < wh16384.adds_level3.size(); i++)
        {
            act.WriteToMem(mbiiBase + wh16384.adds_level3[i], level);
        }
    }
}