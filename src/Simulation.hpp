#ifndef _SIMULATION_HPP
#define _SIMULATION_HPP

#include "Config.hpp"

struct Domain;
struct Particle;

struct Simulation {
    Simulation();
    void tick(double dt);
    void sync();
    bool insert(const Particle& p, Domain* previousDomain = 0);

    // Members
    Domain** domains;
    unsigned int domainCount;
};

#endif