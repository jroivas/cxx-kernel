#pragma once

#include "random.h"

class RandomX86 : public Random
{
public:
    RandomX86();
    virtual unsigned int getRandomInt();

private:
    bool hw_rand;
};
