#include <stdio.h>
#include <omp.h>
#include <list>
#include <cmath>
#include <unistd.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Simulation.hpp"
#include "Particle.hpp"
#include "Vector.hpp"
#include "Domain.hpp"

double drand() {
    return ((double)rand()/(double)RAND_MAX);
}

int main() {
    srand((unsigned)time(NULL));

    // SFML
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "FluidSim");
    sf::Clock clock;
    sf::Texture blur;
    blur.loadFromFile("blur.png");
    auto blurSize = blur.getSize();
    sf::Font font;
    font.loadFromFile("DejaVuSans.ttf");

    // Initialize
    Simulation simulation;

    // Status
    std::string status;
    double particlesToSpawn = 0;
    while(window.isOpen()) {
        // Timing
        float odt = clock.restart().asSeconds();
        float dt = fmin(1.0/100, odt);
        status = "";

        // Spawning
        particlesToSpawn += dt * config.emissionRate;
        while(particlesToSpawn > 1) {
            Particle p(Vector(0, config.domainSize * config.height * drand()), Vector(config.domainSize * config.width * 0.5 + drand() * 0.1, 0));
            if(!simulation.insert(p)) break;
            particlesToSpawn--;
        }

        sf::Event event;
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Period) {
                    config.drawParticles = !config.drawParticles;
                } else if(event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }

        simulation.tick(dt);

        window.clear(sf::Color::Black);

        unsigned int i = 0;
        if(config.drawParticles) {
            sf::VertexArray vertices(sf::Quads, simulation.domainCount * config.domainBufferSize * 4);
            for(unsigned int domainIndex = 0; domainIndex < simulation.domainCount; ++domainIndex) {
                Domain* d = simulation.domains[domainIndex];
                for(unsigned int particleIndex = 0; particleIndex < d->particles.count; ++particleIndex) {
                    const Particle& p = d->particles[particleIndex];
                    const Vector& pos = p.pos * 80.0 + Vector(100, 100);
                    const float s = config.particleSize;

                    // const sf::Color color(255 * p.pressure, 255 - 255 * p.pressure, 0);
                    sf::Color color(100, 150, 255, 100);
                    if(i < 3) color = sf::Color::White;

                    vertices[4*i+0] = sf::Vertex(sf::Vector2f(pos.x - s, pos.y - s), color, sf::Vector2f(0, 0));
                    vertices[4*i+1] = sf::Vertex(sf::Vector2f(pos.x + s, pos.y - s), color, sf::Vector2f(blurSize.x, 0));
                    vertices[4*i+2] = sf::Vertex(sf::Vector2f(pos.x + s, pos.y + s), color, sf::Vector2f(blurSize.x, blurSize.y));
                    vertices[4*i+3] = sf::Vertex(sf::Vector2f(pos.x - s, pos.y + s), color, sf::Vector2f(0, blurSize.y));
                    i++;
                }
            }

            sf::RenderStates rs(sf::BlendAdd, sf::Transform::Identity, &blur, 0);
            window.draw(vertices, rs);
        } else {
            sf::Vertex* vertices = new sf::Vertex[simulation.domainCount * config.domainBufferSize];
            for(unsigned int domainIndex = 0; domainIndex < simulation.domainCount; ++domainIndex) {
                Domain* d = simulation.domains[domainIndex];
                for(unsigned int particleIndex = 0; particleIndex < d->particles.count; ++particleIndex) {
                    const Particle& p = d->particles[particleIndex];
                    const Vector& pos = p.pos * 80.0 + Vector(100, 100);
                    vertices[i] = sf::Vertex(sf::Vector2f(pos.x, pos.y), sf::Color::White);
                    i++;
                }
            }
            window.draw(vertices, i, sf::Points);
            delete vertices;
        }
        status += std::to_string(i) + " / " + std::to_string(config.domainBufferSize * simulation.domainCount) + " particles\n";
        status += std::to_string(1/odt) + " FPS\n";

        sf::Text text(status, font);
        text.setPosition(10, 10);
        text.setCharacterSize(12);
        window.draw(text);

        window.display();
    }
}
