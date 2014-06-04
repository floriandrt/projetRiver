#ifndef POINT_H
#define POINT_H

#include <iostream>

class Point{
    private:
        int n; //dimension du point
        double *data;
    public:
        Point(): n(0), data(NULL){}
        Point(int dim);
        Point(const Point& p);
        ~Point();
        int getSize() const;
        friend std::ostream& operator<<( std::ostream &flux, Point const& p );
        double operator [](int i) const;
        double& operator [](int i);
        Point& operator =(Point const& p);
        Point operator+(Point const& p);
        Point operator+(int val);
        Point operator-(Point const& p);
        Point operator-(int val);
        Point& operator +=(Point const& p);
        Point& operator +=(int val);
        Point& operator -=(Point const& p);
        Point& operator -=(int val);
        Point& operator /=(Point const& p);
        Point& operator *=(Point const& p);
        bool operator !=(Point const& p);
        bool operator <(Point const& p);
        bool operator >(Point const& p);
        bool operator ==(Point const& p);
        static double* intersection(Point seg1, Point seg2);
};

#endif // POINT_H
