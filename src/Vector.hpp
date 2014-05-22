#ifndef _VECTOR_HPP
#define _VECTOR_HPP

struct Vector {
    Vector();
    Vector(double _x, double _y);
    Vector(const Vector& o);

    double dot(const Vector& o) const;
    Vector operator* (double fac) const;
    Vector operator+ (const Vector& o) const;
    void operator+= (const Vector& o);
    void operator*= (double o);
    void operator*= (const Vector& o);
    Vector operator- () const;
    Vector operator- (Vector o) const;
    double len2() const;
    double len() const;
    Vector operator/ (double quo) const;
    Vector normalize() const;

    // Members
    double x = 0;
    double y = 0;
};

#endif