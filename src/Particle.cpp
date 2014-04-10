#include "Particle.hpp"

Particle::Particle() {}

Particle::Particle(const Vector& p, const Vector& v)
    : pos(p)
    , vel(v)
{}
    