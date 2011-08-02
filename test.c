#include <iostream>

struct A {
    double x;

    A() : x(0.0) { }
    A(double x) : x(x) { }

    A operator+ (const A& other) {
        return A(x + other.x);
    }

    A operator- (const A& other) {
        return *this + (-other);
    }

    A operator- () {
        return A(-x);
    }
};

int main(int argc, char *argv[]) {
    A a1 = A(1.0),
      a2 = A(2.0);

    cout << "Values are:" << endl
         << a1.x << endl
         << a2.x << endl
         << (a1 - a2).x << endl;
}
