#include <stdio.h>
#include <string.h>
#include "point.h"
#include <cfloat>

using namespace std;

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

Point::Point(int dim){
//    cout << "construction point taille : " << dim << endl;
    n = dim;
    data = new double[dim];
}

Point::Point(const Point& p){
//     cout << "recopie d'un point" << endl;
    n = p.n;
    data = new double[n];
    for(int i = 0; i<n; i++){
        data[i] = p.data[i];
    }
}

Point::~Point(){
//     cout << "destruction de point" << endl;
    delete[] data;
}

int Point::getSize() const{
    return n;
}

ostream& operator<<( ostream &flux, Point const& p ){
    flux << "(";
    for(int i = 0; i<p.n-1; i++){
        flux << p.data[i] << ", ";
    }
    flux << p.data[p.n-1] << ")";
    return flux;
}

double Point::operator[](int i) const{
    if(i<0 || i>=n){
        cerr << "Index not in the range : " << i << ", limite : " << n << endl;
    }
    return data[i];
}

double& Point::operator[](int i){
    if(i<0 || i>=n){
        cerr << "Index not in the range : " << i << ", limite : " << n << endl;
    }
    return data[i];
}

Point& Point::operator=(Point const& p){
    if(n == 0){
        delete[] data;
        data = new double[p.n];
        n = p.n;
    }
    if(p.n != n){
        cerr << "Points are not the same size op =" << endl;
        cerr << "taille : " << n << ", " << p.n << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] = p.data[i];
    }
    return *this;
}

Point Point::operator+(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size op +" << endl;
        cerr << "taille : " << n << ", " << p.n << endl;
    }
    Point res(n);
    for(int i = 0; i<n; i++){
        res[i] = data[i] + p.data[i];
    }
    return res;
}

Point Point::operator+(int val){
    Point res(n);
    for(int i = 0; i<n; i++){
        res[i] = data[i] + val;
    }
    return res;
}

Point Point::operator-(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    Point res(n);
    for(int i = 0; i<n; i++){
        res[i] = data[i] - p.data[i];
    }
    return res;
}

Point Point::operator-(int val){
    Point res(n);
    for(int i = 0; i<n; i++){
        res[i] = data[i] - val;
    }
    return res;
}

Point& Point::operator+=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] += p.data[i];
    }
    return *this;
}

Point& Point::operator+=(int val){
    for(int i = 0; i<n; i++){
        data[i] += val;
    }
    return *this;
}

Point& Point::operator-=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] -= p.data[i];
    }
    return *this;
}

Point& Point::operator-=(int val){
    for(int i = 0; i<n; i++){
        data[i] -= val;
    }
    return *this;
}

Point& Point::operator/=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] /= p.data[i];
    }
    return *this;
}

Point& Point::operator*=(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        data[i] *= p.data[i];
    }
    return *this;
}

bool Point::operator !=(Point const& p){
    if(n != p.n){
        cerr << "comparaison de 2 points de différente dimension" << endl;
    }
    for(int i = 0; i<n; i++){
        if(data[i] != p.data[i]){
            return true;
        }
    }
    return false;
}

bool Point::operator<(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        if(data[i] >= p.data[i] ){
            return false;
        }
    }
    return true;
}

bool Point::operator>(Point const& p){
    if(p.n != n){
        cerr << "Points are not the same size" << endl;
    }
    for(int i = 0; i<n; i++){
        if(data[i] <= p.data[i] ){
            return false;
        }
    }
    return true;
}

bool Point::operator ==(Point const& p){
    if(p.n != n){
        return false;
    }
    for(int i = 0; i<n; i++){
        if(data[i] != p.data[i]){
            return false;
        }
    }
    return true;
}

//prérequis segX[0] < segX[2]
double* Point::intersection(Point seg1, Point seg2){
    /*
    cerr << "INTERSECTION ENTREE" << endl;
    cerr << "seg1 x1 " << seg1[0] << ", seg1 y1 " << seg1[1] << ", seg1 x2 " << seg1[2] << ", seg1 y2 " << seg1[3] << endl;
    cerr << "seg2 x1 " << seg2[0] << ", seg2 y1 " << seg2[1] << ", seg2 x2 " << seg2[2] << ", seg2 y2 " << seg2[3] << endl;
    */
    double a1;
    double b1;
    double resX = 0;
    double resY;
    double* res = new double[2];
    if(seg1[0] != seg1[2]){
        a1 = seg1[1]-seg1[3];
        a1 /= seg1[0]-seg1[2];
        b1 = seg1[1]-(a1*seg1[0]);
    }else{
        a1 = DBL_MAX;
        b1 = seg1[0];
        resX = seg1[0];
    }
    //cerr << "a1 " << a1 << ", b1 " << b1 << endl;
    double a2;
    double b2;
    if(seg2[0] != seg2[2]){
        a2 = seg2[1]-seg2[3];
        a2 /= seg2[0]-seg2[2];
        b2 = seg2[1]-(a2*seg2[0]);
    }else{
        a2 = DBL_MAX;
        b2 = seg2[0];
        resX = seg2[0];
    }
    //cerr << "a2 " << a2 << ", b2 " << b2 << endl;
    if(a2 == a1){
        //cerr << "INTERSECTION SORTIE NOT FIND: PARALLELE" << endl;
        res[0] = -1;
        res[1] = -1;
        return res;
    }
    //voir cas perpendiculaire (pente nulle et pente verticale)
    if(a1 != DBL_MAX && a2 != DBL_MAX){
        resX = b1-b2;
        resX /= a2-a1;
        resY = a1*resX+b1;
    }else{
        if(a1 == DBL_MAX){
            resY = a2*resX+b2;
        }else{
            resY = a1*resX+b1;
        }
    }
    double limMinX = MAX(MIN(seg1[0],seg1[2]),MIN(seg2[0],seg2[2]));
    double limMaxX = MIN(MAX(seg1[0],seg1[2]),MAX(seg2[0],seg2[2]));
    double limMinY = MAX(MIN(seg1[1],seg1[3]),MIN(seg2[1],seg2[3]));
    double limMaxY = MIN(MAX(seg1[1],seg1[3]),MAX(seg2[1],seg2[3]));
    if(resX <= limMaxX && resX >= limMinX && resY <= limMaxY && resY >= limMinY){
        //cerr << "INTERSECTION SORTIE FIND" << " resX " << resX << ", resY " << resY << endl;
        res[0] = resX;
        res[1] = resY;
        return res;
    }
    //cerr << "INTERSECTION SORTIE NOT FIND: HORS DES SEGS" << " resX " << resX << ", resY " << resY << endl;
    res[0] = -1;
    res[1] = -1;
    return res;
}
