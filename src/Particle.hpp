#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

#include "Vector.hpp"

struct Particle {
    Particle();
    Particle(const Vector& p, const Vector& v);

    // Members
    Vector pos;
    Vector vel;
    Vector force;
    double pressure = 0.0;
    bool dead = false;
};

#endif