#include "Vector.hpp"

#include <cmath>

Vector::Vector()
    : x(0)
    , y(0)
{}

Vector::Vector(double _x, double _y) 
    : x(_x)
    , y(_y) 
{}

Vector::Vector(const Vector& o) 
    : x(o.x)
    , y(o.y) 
{}

Vector Vector::operator* (double fac) const {
    return Vector(x * fac, y * fac);
}

Vector Vector::operator+ (const Vector& o) const {
    return Vector(x + o.x, y + o.y);
}

void Vector::operator+= (const Vector& o) {
    x += o.x; y += o.y;
}

Vector Vector::operator- () const {
    return Vector(-x, -y);
}

Vector Vector::operator- (Vector o) const {
    return Vector(x - o.x, y - o.y);
}

double Vector::len2() const {
    return x * x + y * y;
}

double Vector::len() const {
    return sqrt(len2());
}

Vector Vector::operator/ (double quo) const {
    return Vector(x / quo, y / quo);
}

Vector Vector::normalize() const {
    return *this / len();
}
