#include "Simulation.hpp"

#include <cstdlib>
#include <cmath>

#include "Particle.hpp"
#include "Domain.hpp"

Simulation::Simulation() 
{
    domainCount = config.width * config.height;
    domains = (Domain**)calloc(domainCount, sizeof(Domain*));
    for(int i = 0; i < domainCount; ++i) {
        domains[i] = new Domain();
    }
}

void Simulation::tick(double dt) {
    #pragma omp parallel for
    for(int i = 0; i < domainCount; ++i) {
        domains[i]->tick(dt);
    }
}

void Simulation::sync() {
    for(int i = 0; i < domainCount; ++i) {
        domains[i]->sync(this);
    }
}

bool Simulation::insert(const Particle& p, Domain* previousDomain) {
    int x = floor(p.pos.x / config.domainSize);
    int y = floor(p.pos.y / config.domainSize);
    if(previousDomain == 0 || (previousDomain->x != x && previousDomain->y != y)) {
        return domains[x + y * config.width]->particles.insert(p);
    }
    return false;
}
