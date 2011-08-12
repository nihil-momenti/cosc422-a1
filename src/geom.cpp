#include <cmath>

#include "geom.hpp"


bool Point::operator== (const Point&  other) {
    return ( (x == other.x) && (y == other.y) && (z == other.z) );
}

bool Point::operator!= (const Point& other) {
    return ! (*this == other);
}

Point Point::operator+ (const Vector& other) {
    return Point(x + other.dx, y + other.dy, z + other.dz);
}

Point Point::operator- (const Vector& other) {
    return (*this + (-other));
}

Vector Point::operator- (const Point& other) {
    return Vector(x - other.x, y - other.y, z - other.z);
}

std::ostream& operator<< (std::ostream& o, const Point& point) {
    return o << "(" << point.x << ", " << point.y << ", " << point.z << ")";
}



double Vector::length() {
    return sqrt(this->dot(*this));
}

double Vector::dot(const Vector& other) {
    return (dx * other.dx) + (dy * other.dy) + (dz * other.dz);
}

Vector Vector::cross(const Vector& other) {
    return Vector(
        dy * other.dz - dz * other.dy,
        dz * other.dx - dx * other.dz,
        dx * other.dy - dy * other.dx
    );
}

Vector Vector::unit() {
    return *this / this->length();
}

bool Vector::operator== (const Vector& other) {
    return ( (dx == other.dx) && (dy == other.dy) && (dz == other.dz) );
}

bool Vector::operator!= (const Vector& other) {
    return ! (*this == other);
}

Vector Vector::operator+ (const Vector& other) {
    return Vector(dx + other.dx, dy + other.dy, dz + other.dz);
}

Vector Vector::operator- (const Vector& other) {
    return *this + (-other);
}

Vector Vector::operator* (double other) {
    return Vector(dx * other, dy * other, dz * other);
}

Vector Vector::operator/ (double other) {
    return Vector(dx / other, dy / other, dz / other);
}
