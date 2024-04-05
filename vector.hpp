
#ifndef vect
#include <iostream>
class PosVector {
    private:
    
    public:
    float x, y;
    PosVector(float x, float y);

    float getX() const;
    float getY() const;

    float getNorm() const;

    void setX(float x);
    void setY(float y);
    void print() const;

    PosVector normal() const;

    PosVector operator +(PosVector &v);
    PosVector operator -(PosVector &v);

    PosVector operator *(const float a); 
    float operator *(PosVector &v) const;
    PosVector operator /(const float a);

    PosVector& operator +=(PosVector& v);
    PosVector& operator -=(PosVector& v);

    PosVector& operator *=(float v);
    PosVector& operator /=(float v);

    bool operator ==(PosVector& v) const;
    bool operator !=(PosVector& v) const;
};

#define vect 
#endif