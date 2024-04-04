
#ifndef vect
class PosVector {
    private:
    int x, y;
    
    public:
    PosVector(int x, int y);

    int getX();
    int getY();


    void setX(int x);
    void setY(int y);
    void print();

    PosVector operator +(PosVector &v);
    PosVector operator -(PosVector &v);

    PosVector operator *(int a);
    PosVector operator /(int a);

    PosVector& operator +=(PosVector& v);
    PosVector& operator -=(PosVector& v);

    PosVector& operator *=(int v);
    PosVector& operator /=(int v);

    bool operator ==(PosVector& v);
    bool operator !=(PosVector& v);
};

#define vect 
#endif