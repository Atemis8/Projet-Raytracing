#include "vector.hpp"
#include <iostream>
#include <format>

PosVector::PosVector(float x, float y) {
    this->x = x;
    this->y = y;
}

PosVector::PosVector() {
    this->x = 0.0f;
    this->y = 0.0f;
}

float PosVector::getNorm() const {
    return (x * x + y * y);
}

float PosVector::getX() const { return x; }
float PosVector::getY() const { return y; }

void PosVector::setX(float x) { this->x = x; }
void PosVector::setY(float y) { this->y = y; }

void PosVector::print() const {
    printf("(%f, %f) \n", x, y);
}

std::ostream & operator << (std::ostream &out, PosVector &c) {
    return out << "(" << c.x << ", " << c.y << ")";
}

std::istream & operator >> (std::istream &in,  PosVector &c) {
    char space;
    in >> space;
    in >> c.x;
    in >> space;
    in >> c.y;
    in >> space;
    return in;
}

PosVector PosVector::operator +(PosVector &v) {
    return PosVector(x + v.getX(), y + v.getY());
}

PosVector PosVector::operator -(PosVector &v) {
    return PosVector(x - v.getX(), y - v.getY());
}

PosVector PosVector::operator *(const float a) {
    return PosVector((x * a), (y * a));
}

PosVector PosVector::operator /(const float a) {
    return PosVector(x / a, y / a);
}

float PosVector::operator *(PosVector &v) const {
    return (x * v.getX() + y * v.getY());
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

PosVector& PosVector::operator *=(float a) {
    setX(a * x); setY(a * y);
    return *this;
}

PosVector& PosVector::operator /=(float a) {
    setX(x / a); setY(y / a);
    return *this;
}

PosVector PosVector::normal() const {
    return PosVector(y, -x);
}

bool PosVector::operator ==(PosVector& v) const {
    return (x == v.getX() && y == v.getY());
}

bool PosVector::operator !=(PosVector& v) const {
    return !(*this == v);
}  



