#ifndef _CONFIG_HPP
#define _CONFIG_HPP

struct Config {
    double gravity = 0.0;
    double influenceDistance = 0.1;
    double influenceDistanceThreshold = 0.02;
    double influencePower = 2;
    double forceFactor = 25.0;
    int    domainBufferSize = 800;
    double emissionRate = 1000.0;
    float  particleSize = 22.f;
    int    width = 10;
    int    height = 5;
    double domainSize = 1.0;
    
    bool bounce = false;

    // display options
    double simulationSpeed = 1.0;
    bool   drawParticles = false;
    bool   drawDomains = false;

    // runtime information
    int    rank;
    int    clients;
};

extern Config config;

#endif