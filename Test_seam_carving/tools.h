#ifndef TOOLS_H
#define TOOLS_H

#include <vector>
#include "data.h"
#include "point.h"

class Tools{
    public :
        Tools() {}
        static double* average(double **data, int dim, int size);
        static Point averageMulDim(Data& d);
        static Point averageMulDimNorm(Data& d);
        static double* variance(double **data, double mean[], int dim, int size);
        static double** varianceMulDim(Data& d, Point mean);
        static double** varianceMulDimNorm(Data& d, Point mean);
        static double gaussian(double x, double mean, double variance);
        static double generateGaussian(double mean, double variance);
        static Point generateMulDim(Point mean, double **variance);
        static Point generateY(Point mean, double** variance, double x1, double x2, double y1);
        static Point pickInSample(Data* d, double x1, double x2);
        static Point rejet(Point min, Point max, Point mean, double** variance, int limit);
        static Point rejet(Point mean, double** variance, std::vector<Point> sample);
        static bool isGaussian(Point x, std::vector<Point> sample, double longMax);
        static double scaledGenerated(double newValue, Data* d, bool changeY);
};

#endif //TOOLS_H
