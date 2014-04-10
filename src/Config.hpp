#ifndef _CONFIG_HPP
#define _CONFIG_HPP

struct Config {
    double gravity = 0.0;
    double influenceDistance = 0.3;
    double influencePower = 2;
    double forceFactor = 10.0;
    int    domainBufferSize = 800;
    double emissionRate = 1000.0;
    bool   drawParticles = false;
    float  particleSize = 12.f;
    int    width = 10;
    int    height = 5;
    double domainSize = 1.0;
};

extern Config config;

#endif