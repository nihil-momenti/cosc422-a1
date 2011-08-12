#ifndef GEOM_HPP
#define GEOM_HPP

#include <iostream>

struct Vector;

struct Point {
    double x, y, z;

    Point() :
        x(0.0), y(0.0), z(0.0) {
    }

    Point(double x, double y, double z) :
        x(x), y(y), z(z) {
    }

    Point(const Point& other) :
        x(other.x), y(other.y), z(other.z) {
    }

    bool operator== (const Point& other);
    bool operator!= (const Point& other);

    Point operator+ (const Vector& other);
    Point operator- (const Vector& other);

    Vector operator- (const Point& other);

    friend std::ostream& operator<< (std::ostream& o, const Point& point);
};

struct Vector {
    double dx, dy, dz;

    Vector() :
        dx(0.0), dy(0.0), dz(0.0) {
    }

    Vector(double dx, double dy, double dz) :
        dx(dx), dy(dy), dz(dz) {
    }

    Vector(const Vector& other) :
        dx(other.dx), dy(other.dy), dz(other.dz) {
    }

    double length();
    double dot(const Vector& other);
    Vector cross(const Vector& other);
    Vector unit();

    bool operator== (const Vector& other);
    bool operator!= (const Vector& other);

    Vector operator+ (const Vector& other);
    Vector operator- (const Vector& other);

    Vector operator* (double other);
    Vector operator/ (double other);

    friend std::ostream& operator<< (std::ostream& o, const Vector& vector);
};

inline Vector operator- (const Vector& v) {
    return Vector(-v.dx, -v.dy, -v.dz);
}

inline Vector operator* (double lhs, Vector rhs) {
    return rhs * lhs;
}

inline Vector operator/ (double lhs, Vector rhs) {
    return rhs / lhs;
}
#endif
