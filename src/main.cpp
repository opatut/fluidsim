#include <stdio.h>
#include <omp.h>
#include <mpi.h>
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

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &config.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &config.clients);

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

    // for(int i = 0; i < 10000; ++i) {
    //         Particle p(Vector(config.domainSize * config.width * drand(), config.domainSize * config.height * drand()), Vector(0, 0));
    //         simulation.insert(p);
    //     }

    // Status
    std::string status;
    double particlesToSpawn = 0;
    while(window.isOpen()) {
        // Timing
        float odt = clock.restart().asSeconds();
        float dt = fmin(1.0/100, odt);
        dt *= config.simulationSpeed;
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
                } else if(event.key.code == sf::Keyboard::Comma) {
                    config.drawDomains = !config.drawDomains;
                } else if(event.key.code == sf::Keyboard::Add) {
                    config.simulationSpeed += 0.1;
                } else if(event.key.code == sf::Keyboard::Subtract) {
                    config.simulationSpeed -= 0.1;
                } else if(event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }

        simulation.tick(dt);

        window.clear(sf::Color::Black);

        double scale = 80;
        Vector offset(100, 100);

        // draw domains
        if(config.drawDomains) {
            sf::VertexArray domainLines(sf::Lines, (config.width + config.height + 2) * 2);
            sf::Color domainLineColor(255, 255, 255, 20);
            for(unsigned int x = 0; x <= config.width; x++) {
                const Vector& start = Vector(x * config.domainSize, 0) * scale + offset;
                const Vector& end = Vector(x * config.domainSize, config.height * config.domainSize) * scale + offset;
                domainLines.append(sf::Vertex(sf::Vector2f(start.x, start.y), domainLineColor));
                domainLines.append(sf::Vertex(sf::Vector2f(end.x, end.y), domainLineColor));
            }
            for(unsigned int y = 0; y <= config.height; y++) {
                const Vector& start = Vector(0, y * config.domainSize) * scale + offset;
                const Vector& end = Vector(config.width * config.domainSize, y * config.domainSize) * scale + offset;
                domainLines.append(sf::Vertex(sf::Vector2f(start.x, start.y), domainLineColor));
                domainLines.append(sf::Vertex(sf::Vector2f(end.x, end.y), domainLineColor));
            }
            window.draw(domainLines);
        }

        unsigned int i = 0;
        if(config.drawParticles) {
            sf::VertexArray vertices(sf::Quads, simulation.domainCount * config.domainBufferSize * 4);
            for(unsigned int domainIndex = 0; domainIndex < simulation.domainCount; ++domainIndex) {
                Domain* d = simulation.domains[domainIndex];
                for(unsigned int particleIndex = 0; particleIndex < d->particles.count; ++particleIndex) {
                    const Particle& p = d->particles[particleIndex];
                    const Vector& pos = p.pos * scale + offset;
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
                    const Vector& pos = p.pos * scale + offset;
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

    MPI_Finalize();
}
