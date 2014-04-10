#ifndef _DOMAIN_HPP
#define _DOMAIN_HPP

#include "Config.hpp"
#include "QuickSet.hpp"
#include "Particle.hpp"
#include "Simulation.hpp"

struct Domain {
    Domain();
    void tick(double dt);
    void sync(Simulation* sim);

    // Members
    QuickSet<Particle> particles;
    int x; 
    int y;
};

#endif