#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <omp.h>
#include <list>
#include <cmath>
#include <unistd.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

// #define GRAVITY 9.81
#define INFLUENCE_DIST 0.2
#define INFLUENCE_POW 2
#define FORCE_FACTOR 10
#define BUFSIZE 8000
#define RATE 3000
#define BOX 4
#define DRAW_PARTICLES 0

struct Vector {
    Vector() = default;
    Vector(double _x, double _y) : x(_x), y(_y) {}
    Vector(const Vector& o) : x(o.x), y(o.y) {}

    double x = 0;
    double y = 0;

    Vector operator* (double fac) const {
        return Vector(x * fac, y * fac);
    }

    Vector operator+ (const Vector& o) const {
        return Vector(x + o.x, y + o.y);
    }

    void operator+= (const Vector& o) {
        x += o.x; y += o.y;
    }

    Vector operator- () const {
        return Vector(-x, -y);
    }

    Vector operator- (Vector o) const {
        return Vector(x - o.x, y - o.y);
    }

    double len2() const {
        return x * x + y * y;
    }

    double len() const {
        return sqrt(len2());
    }

    Vector operator/ (double quo) const {
        return Vector(x / quo, y / quo);
    }

    Vector normalize() const {
        return *this / len();
    }
};

struct BarrierSegment {
    Vector pos;
    Vector dir;
};

struct Particle {
    Particle() = default;
    Particle(Vector p, Vector v) : pos(p), vel(v) {}

    Vector pos;
    Vector vel;
    Vector force;
    double pressure;
    bool dead = false;
};

template <typename T>
struct QuickSet {
    unsigned int count = 0;
    unsigned int size;
    T* buf;

    QuickSet(unsigned int s) {
        size = s; 
        buf = (T*)calloc(size, sizeof(T));
    }

    void remove(unsigned int index) {
        // move last element to index position
        memcpy(&buf[index], &buf[count-1], sizeof(T));
        // shorten array by one
        count--;
    }

    T* allocate() {
        assert(count < size);
        count++;
        return &buf[count-1];
    }

    T& insert(T&& item) {
        T* t = allocate();
        memcpy(t, &item, sizeof(T));
        return *t;
    }

    T& operator[](int index) {
        assert(index < count && index >= 0);
        return buf[index];   
    }
};

struct Domain {
    QuickSet<Particle> particles = QuickSet<Particle>(BUFSIZE);

    void tick(double dt) {
        for(unsigned int i = 0; i < particles.count; ++i) {
            particles[i].force = Vector(0, 0);
            particles[i].pressure = 0;
        }

        #pragma omp parallel for
        for(unsigned int i = 0; i < particles.count; ++i) {
            Particle& subject = particles[i];
            for(unsigned int j = i + 1; j < particles.count; ++j) {
                Particle& object = particles[j];

                Vector diff = subject.pos - object.pos;
                double strength = 1 - diff.len() / INFLUENCE_DIST;
                strength = fmin(1, fmax(0, strength));
                strength = pow(strength, INFLUENCE_POW);
                Vector force = diff.normalize() * strength * FORCE_FACTOR;
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

            if(p.pos.x == 0 && p.pos.y == 0) {
                printf("MEH\n");
            }
            p.pos = p.pos + p.vel * dt;

            double b = BOX;
            double h = BOX * 0.5;
            // if(p.pos.x < -b) { p.pos.x += 2*b; }
            // if(p.pos.x >  b) { p.pos.x -= 2*b; }
            // if(p.pos.y < -b) { p.pos.y += 2*b; }
            // if(p.pos.y >  b) { p.pos.y -= 2*b; }

            if(p.pos.x < -b) { p.dead = true; }
            if(p.pos.x >  b) { p.dead = true; }
            // if(p.pos.y < -b) { p.dead = true; }
            // if(p.pos.y >  b) { p.dead = true; }

            // if(p.pos.x < -b) { p.pos.x = -2*b - p.pos.x; p.vel.x *= -0.2; }
            // if(p.pos.x >  b) { p.pos.x =  2*b - p.pos.x; p.vel.x *= -0.2; }
            if(p.pos.y < -h) { p.pos.y = -2*h - p.pos.y; p.vel.y *= -0.2; }
            if(p.pos.y >  h) { p.pos.y =  2*h - p.pos.y; p.vel.y *= -0.2; }

            // if(p.pos.x < -b) { p.pos.x = -b; p.vel.x = 0; }
            // if(p.pos.x >  b) { p.pos.x =  b; p.vel.x = 0; }
            // if(p.pos.y < -b) { p.pos.y = -b; p.vel.y = 0; }
            // if(p.pos.y >  b) { p.pos.y =  b; p.vel.y = 0; }

            if(p.pos.len() < 1) {
                p.pos = p.pos.normalize();
            }
        }

        for(int i = particles.count - 1; i >= 0; --i) {
            if(particles[i].dead) {
                particles.remove(i);
            }
        }
    }
};

double drand() {
    return ((double)rand()/(double)RAND_MAX);
}

int main() {
    srand((unsigned)time(NULL));

    Domain d;
    // Particle& p1 = d.particles.insert(Particle(Vector(-1, 0), Vector(1, 0)));
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "FluidSim");
    sf::Texture blur;
    sf::Clock clock;
    blur.loadFromFile("blur.png");
    auto blurSize = blur.getSize();

        // for(unsigned int i = d.particles.count; i < BUFSIZE; ++i) {
        //     d.particles.insert(Particle(Vector(2 * drand() - 1, 2 * drand() - 1), Vector(1, 0)));
        // }

    double particlesToSpawn = 0;

    while(window.isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = fmin(1.0/100, dt);

        particlesToSpawn += dt * RATE;

        while(particlesToSpawn > 1 && d.particles.count < d.particles.size) {
            d.particles.insert(Particle(Vector(-BOX + dt * drand() * BOX, BOX * 2 * drand() - BOX), Vector(BOX + drand() * 0.1, 0)));
            particlesToSpawn--;
        }

        sf::Event event;
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        d.tick(dt);

        window.clear(sf::Color::Black);

#if DRAW_PARTICLES
        sf::VertexArray vertices(sf::Quads, BUFSIZE * 4);
        for(unsigned int i = 0; i < d.particles.count; ++i) {
            const Particle& p = d.particles[i];
            const Vector& pos = p.pos * 80.0 + Vector(500, 500);
            const double s = 8;

            // const sf::Color color(255 * p.pressure, 255 - 255 * p.pressure, 0);
            sf::Color color(100, 150, 255, 100);
            if(i < 3) color = sf::Color::White;

            vertices[4*i+0] = sf::Vertex(sf::Vector2f(pos.x - s, pos.y - s), color, sf::Vector2f(0, 0));
            vertices[4*i+1] = sf::Vertex(sf::Vector2f(pos.x + s, pos.y - s), color, sf::Vector2f(blurSize.x, 0));
            vertices[4*i+2] = sf::Vertex(sf::Vector2f(pos.x + s, pos.y + s), color, sf::Vector2f(blurSize.x, blurSize.y));
            vertices[4*i+3] = sf::Vertex(sf::Vector2f(pos.x - s, pos.y + s), color, sf::Vector2f(0, blurSize.y));
        }

        sf::RenderStates rs(sf::BlendAdd, sf::Transform::Identity, &blur, 0);
        window.draw(vertices, rs);
#else 
        sf::Vertex vertices[BUFSIZE];
        for(unsigned int i = 0; i < d.particles.count; ++i) {
            const Particle& p = d.particles[i];
            const Vector& pos = p.pos * 80.0 + Vector(500, 500);
            vertices[i] = sf::Vertex(sf::Vector2f(pos.x, pos.y), sf::Color::White);
        }
        window.draw(vertices, d.particles.count, sf::Points);
#endif
        window.display();
        printf("%.4f\n", 1/dt);
    }
}
