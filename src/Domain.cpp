#include "Domain.hpp"

#include <cmath>

Domain::Domain() 
    : particles(config.domainBufferSize)
{}

void Domain::tick(double dt) {
    for(unsigned int i = 0; i < particles.count; ++i) {
        particles[i].force = Vector(0, 0);
        particles[i].pressure = 0;
    }

    for(unsigned int i = 0; i < particles.count; ++i) {
        Particle& subject = particles[i];
        for(unsigned int j = i + 1; j < particles.count; ++j) {
            Particle& object = particles[j];

            Vector diff = subject.pos - object.pos;
            double strength = 1 - diff.len() / config.influenceDistance;
            strength = fmin(1, fmax(0, strength));
            strength = pow(strength, config.influencePower);
            Vector force = diff.normalize() * strength * config.forceFactor;
            subject.force += force;
            object.force += -force;
            subject.pressure += force.len();
            object.pressure += force.len();
        }

        #ifdef GRAVITY
            subject.vel += Vector(0, GRAVITY) * dt;
        #endif
    }

    for(unsigned int i = 0; i < particles.count; ++i) {
        Particle& p = particles[i];
        p.vel += p.force * dt;
        p.vel = p.vel * (1 - dt * 0.1);
        p.pressure = p.force.len();
        p.pos = p.pos + p.vel * dt;

        double w = config.domainSize * config.width;
        double h = config.domainSize * config.height;
        // if(p.pos.x < -w) { p.pos.x += 2*w; }
        // if(p.pos.x >  w) { p.pos.x -= 2*w; }
        // if(p.pos.y < -w) { p.pos.y += 2*w; }
        // if(p.pos.y >  w) { p.pos.y -= 2*w; }

        if(p.pos.x < 0) { p.dead = true; }
        if(p.pos.x >  w) { p.dead = true; }
        // if(p.pos.y < -w) { p.dead = true; }
        // if(p.pos.y >  w) { p.dead = true; }

        // if(p.pos.x < -w) { p.pos.x = -2*w - p.pos.x; p.vel.x *= -0.2; }
        // if(p.pos.x >  w) { p.pos.x =  2*w - p.pos.x; p.vel.x *= -0.2; }
        if(p.pos.y < 0) { p.pos.y = -2*h - p.pos.y; p.vel.y *= -0.2; }
        if(p.pos.y >  h) { p.pos.y =  2*h - p.pos.y; p.vel.y *= -0.2; }

        // if(p.pos.x < -w) { p.pos.x = -w; p.vel.x = 0; }
        // if(p.pos.x >  w) { p.pos.x =  w; p.vel.x = 0; }
        // if(p.pos.y < -w) { p.pos.y = -w; p.vel.y = 0; }
        // if(p.pos.y >  w) { p.pos.y =  w; p.vel.y = 0; }

        // Circle in center
        Vector circlePos(w * 0.5, h * 0.5);
        Vector diff = p.pos - circlePos;
        if(diff.len() < 1) {
            p.pos = circlePos + diff.normalize();
        }
    }

    for(int i = particles.count - 1; i >= 0; --i) {
        if(particles[i].dead) {
            particles.remove(i);
        }
    }
}

void Domain::sync(Simulation* sim) {
    for(int i = particles.count - 1; i >= 0; --i) {
        Particle& p = particles[i];
        if(sim->insert(p, this)) {
            particles.remove(i);
        }
    }
}

