#ifndef _SYNCHRONIZER_HPP
#define _SYNCHRONIZER_HPP

#include "Config.hpp"

struct Particle;
struct Domain;

struct Synchronizer {
    Particle** pushQueueByRank;

    Synchronizer();
    static int getClientByDomain(Domain* d);
};

#endif