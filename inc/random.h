#ifndef _RANDOM_H
#define _RANDOM_H

class Random
{
public:
    Random();

    virtual void init();
    virtual unsigned int getRandomInt();

private:
    unsigned int rand_seed;
};

#endif
