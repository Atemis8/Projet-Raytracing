#include "vector.hpp"
#include <iostream>
#include <format>

PosVector::PosVector(int x, int y) {
    this->x = x;
    this->y = y;
}

int PosVector::getX() { return x; }
int PosVector::getY() { return y; }

void PosVector::setX(int x) { this->x = x; }
void PosVector::setY(int y) { this->y = y; }

void PosVector::print() {
    printf("(%d, %d) \n", x, y);
}

PosVector PosVector::operator +(PosVector &v) {
    return PosVector(x + v.getX(), y + v.getY());
}

PosVector PosVector::operator -(PosVector &v) {
    return PosVector(x - v.getX(), y - v.getY());
}

PosVector PosVector::operator *(int a) {
    return PosVector(x * a, y * a);
}
PosVector PosVector::operator /(int a) {
    return PosVector(x / a, y / a);
}

PosVector& PosVector::operator +=(PosVector& v) {
    setX(x + v.getX());
    setY(y + v.getY());
    return *this;
}

PosVector& PosVector::operator -=(PosVector& v) {
    setX(x - v.getX());
    setY(y - v.getY());
    return *this;
}

PosVector& PosVector::operator *=(int a) {
    setX(a * x); setY(a * y);
    return *this;
}

PosVector& PosVector::operator /=(int a) {
    setX(x / a); setY(y / a);
    return *this;
}

bool PosVector::operator ==(PosVector& v) {
    return (x == v.getX() && y == v.getY());
}

bool PosVector::operator !=(PosVector& v) {
    return !(*this == v);
}



