#include "random.h"
#include "platform.h"

Random::Random()
{
    init();
}

void Random::init()
{
    rand_seed = Platform::timer()->getTicks();
}

unsigned int Random::getRandomInt()
{
    // Crappy and simple random number gen
    rand_seed = rand_seed * 1103515245 + 12345;
    return (unsigned int)(rand_seed / 65536) % 32768;
}
