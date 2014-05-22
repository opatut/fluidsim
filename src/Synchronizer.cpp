#include "Synchronizer.hpp"

#include "Particle.hpp"
#include "Domain.hpp"

Synchronizer::Synchronizer() {
    pushQueueByRank = (Particle**) malloc(sizeof(Particle*) * config.clients);
}

int Synchronizer::getClientByDomain(Domain* d) {
    // cast to int floors
    return (int)(d->y * config.clients / config.height);
}
