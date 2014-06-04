#ifndef DATA_H
#define DATA_H

#include <iostream>
#include "point.h"
#include <vector>

class Data{
    private:
        std::vector<Point> data; //contient les données (segment)
        std::vector<Point> normData; //les mêmes données normalisées
        std::vector<Point> sample; //échantillon généré grâce aux données
        Point mean; //moyenne des données
        double** var; //matrice de covariance des données
        int posInsert; //position d'insertion d'une nouvelle donnée
        //coordonnées de la boîte contenant les données
        int minX;
        int maxX;
        int minY;
        int maxY;
        double longMax; //longueur du segment le plus long
        Point target;
    public:
        Data(): data(), sample(), mean(0), var(NULL), posInsert(-1), minX(0), maxX(0), minY(0), maxY(0), longMax(0), target(0){}
        Data(const Data& d);
        ~Data();
        void erase(int index);
        void insert(Point p, int index);
        static Point normalize4D(Point p);
        void addData(Point newData);
        void eraseData(int delta);
        int reduceData(int delta, int index, bool& doClear);
        void updateDate(int delta);
        void insertData4D(Point newData, double *pos, int ind);
        int getDataSize() const;
        bool isGaussian();
        Point getMean();
        double** getVar();
        std::vector<Point> getSample();
        void setMean(Point m);
        void setVar(double** v);
        void addSample(Point value);
        int getPosInsert();
        void setPosInsert(int pos);
        int xMin();
        int xMax();
        int yMin();
        int yMax();
        void updateX(double newX);
        void updateY(double newY);
        void shiftMinX(double delta);
        void shiftMaxX(double delta);
        void setSample(std::vector<Point> newSample);
        Point getSample(int i);
        int getSampleSize();
        Point getNormData(int i);
        double getLongMax();
        void setTarget(Point p);
        Point getTarget();
        Point operator[](int i) const;
        Point& operator[](int i);
        friend std::ostream& operator<<( std::ostream &flux, Data const& d );
};

#endif // DATA_H
