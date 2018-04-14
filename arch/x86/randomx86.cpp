#include "randomx86.h"

extern "C" int hasHwRandom();
extern "C" unsigned int getHwRandom();

RandomX86::RandomX86()
{
    hw_rand = hasHwRandom();
}

unsigned int RandomX86::getRandomInt()
{
    if (hw_rand) return getHwRandom();

    // Fallback to shitty hw gen
    return Random::getRandomInt();
}
