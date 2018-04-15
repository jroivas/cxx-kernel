#include "random.h"
#include "platform.h"

Random::Random()
{
    rand_seed = 0;
}

void Random::init()
{
    if (rand_seed != 0) return;
    rand_seed = Platform::timer()->getTicks();
}

unsigned int Random::getRandomInt()
{
    init();
    // Crappy and simple random number gen
    rand_seed = rand_seed * 1103515245 + 12345;
    return (unsigned int)(rand_seed / 65536) % 32768;
}
