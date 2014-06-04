#include "tools.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cfloat>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

using namespace std;

Point mult(double** const A, Point x){
    Point res(x.getSize());
    for(int i = 0; i<x.getSize(); i++){
        res[i] = 0;
        for(int j = 0; j<x.getSize(); j++){
            res[i] += A[i][j]*x[j];
        }
    }
    return res;
}

double** add(double** A, int dim, double val){
    double** res = new double*[dim];
    for(int i = 0; i<dim; i++){
        res[i] = new double[dim];
        for(int j = 0; j<dim; j++){
            res[i][j] = (i==j) ? A[i][j]+val : A[i][j];
        }
    }
    return res;
}

//méthode de calcul d'une racine carrée d'une matrice
double** cholesky(double** A, int size){
    double** L = new double*[size];
    double sum;
    double prod = 0;

    for(int i = 0; i<size; i++){
        sum = 0;
        L[i] = new double[size];
        for(int j = 0; j<size; j++){
            if(i>j){
                prod = 0;
                for(int k = 0; k<j; k++){
                    prod += L[j][k]*L[i][k];
                }
                L[i][j] = A[j][i]-prod;
                L[i][j] /= L[j][j];
                sum += pow(L[i][j],2);
            }else if(i == j){
                L[i][i] = sqrt(A[i][i]-sum);
                double epsilon = 0.0000001;
                if(abs(L[i][i]) < epsilon){
                    return cholesky(add(A,size,epsilon),size);
                }
            }else{
                L[i][j] = 0;
            }
        }
    }
    return L;
}

double* Tools::average(double **data, int dim, int size){
    double *result = new double[dim];
    for(int i = 0; i<dim; i++){
        result[i] = 0;
        for(int j = 0; j<size; j++){
            result[i] += data[i][j];
        }
        result[i] /= size;
    }
    return result;
}

Point Tools::averageMulDim(Data& d){
    int dim = d[0].getSize();
    Point res(dim);
    for(int i = 0; i<dim; i++){
        res[i] = 0;
        for(int j = 0; j<d.getDataSize(); j++){
            res[i] += d[j][i];
        }
        res[i] /= d.getDataSize();
    }
    return res;
}

Point Tools::averageMulDimNorm(Data& d){
    int dim = d[0].getSize();
    Point res(dim);
    for(int i = 0; i<dim; i++){
        res[i] = 0;
        for(int j = 0; j<d.getDataSize(); j++){
            res[i] += (d.getNormData(j))[i];
        }
        res[i] /= d.getDataSize();
    }
    return res;
}

double* Tools::variance(double **data, double mean[], int dim, int size){
    double *result = new double[dim];
    for(int i = 0; i<dim; i++){
        result[i] = 0;
        for(int j = 0; j<size; j++){
            result[i] += (data[i][j]*data[i][j])-(mean[i]*mean[i]);
        }
        result[i] /= size;
        result[i] = sqrt(result[i]);
    }
    return result;
}

double** Tools::varianceMulDim(Data& d, Point mean){
    int dim = mean.getSize();
    double** res = new double*[dim];
    for(int i = 0; i<dim; i++){
        res[i] = new double[dim];
        for(int j = 0; j<dim; j++){
            res[i][j] = 0;
            for(int k = 0; k<d.getDataSize(); k++){
                res[i][j] += (d[k][i]-mean[i])*(d[k][j]-mean[j]);
            }
            res[i][j] /= (d.getDataSize()-1);
        }
    }
    return res;
}

double** Tools::varianceMulDimNorm(Data& d, Point mean){
    int dim = mean.getSize();
    double** res = new double*[dim];
    for(int i = 0; i<dim; i++){
        res[i] = new double[dim];
        for(int j = 0; j<dim; j++){
            res[i][j] = 0;
            for(int k = 0; k<d.getDataSize(); k++){
                res[i][j] += ((d.getNormData(k))[i]-mean[i])*((d.getNormData(k))[j]-mean[j]);
            }
            res[i][j] /= (d.getDataSize()-1);
        }
    }
    return res;
}

double Tools::gaussian(double x, double mean, double variance){
    return (1/(sqrt(2*M_PI)*variance))*exp(-0.5*pow((x-mean)/variance,2));
}

double Tools::generateGaussian(double mean, double variance){
    double temp = sqrt(-2*log((double)rand()/RAND_MAX))*cos(2*M_PI*(double)rand()/RAND_MAX);
    return temp*variance+mean;
}

Point Tools::generateMulDim(Point mean, double **variance){
    if(variance == NULL){
        cerr << "Matrice de variance NULLE" << endl;
    }
    int dim = mean.getSize();
    double **A = cholesky(variance,dim);
    /*
    for(int i = 0; i<dim; i++){
        for(int j = 0; j<dim; j++){
            cerr << " | " << A[i][j];
        }
        cerr << endl;
    }
    cerr << endl;
    for(int i = 0; i<dim; i++){
        for(int j = 0; j<dim; j++){
            cerr << " | " << variance[i][j];
        }
        cerr << endl;
    }
    */
    //x suit une loi N(0,I)
    Point x(dim);
    //y suit une loi N(mean,variance)
    Point y(dim);
    for(int i = 0; i<dim; i++){
        x[i] = Tools::generateGaussian(0,1);
    }
    y = mult(A,x);
    y += mean;
//    cerr << "donnee generee inside : " << y[0] << ", " << y[1] << ", " << y[2] << ", " << y[3] << endl;
    return y;
}

Point Tools::generateY(Point mean, double** variance, double x1, double x2, double y1){
    Point y = generateMulDim(mean, variance);
    cout << "y : " << y << endl;
    y[0] = x1;
    y[1] = y1;
    y[2] = x2;
    y[3] += y1;
    return y;
}

Point Tools::pickInSample(Data* d, double x1, double x2){
    Point y = d->getSample(rand() % (d->getSampleSize()-1));
    y[0] = x1;
    y[2] = x2;
    return y;
}

Point Tools::rejet(Point min, Point max, Point mean, double** variance, int limit){
    Point test = generateMulDim(mean, variance);
    for(int i = 0; i<limit; i++){
        if(test > min && test < max){
            return test;
        }
    }
    return Point();
}

Point Tools::rejet(Point mean, double** variance, vector<Point> sample){
    Point test = generateMulDim(mean, variance);
    int threshold = 1;
    for(int i = 0; i<(int)sample.size(); i++){
        if(test < (sample[i]+threshold) && test > (sample[i]-threshold)){
            return test;
        }
    }
    return Point();
}

bool Tools::isGaussian(Point x, vector<Point> sample, double longMax){
    int threshold = 1;
    Point normX = Data::normalize4D(x);
    for(int i = 0; i<(int)sample.size(); i++){
        if((normX[2]*normX[2]+normX[3]*normX[3]) > longMax){
            return true;
        }
        if(normX < (sample[i]+threshold) && normX > (sample[i]-threshold)){
            return true;
        }
    }
    return false;
}

double Tools::scaledGenerated(double newValue, Data* d, bool changeY){
    Point target = d->getTarget();
    double longX = target[2] /*- target[0]*/;
    double longY = target[3] /*- target[1]*/;
    double pente = (longX == 0) ? DBL_MAX : longY/longX;
    if(changeY){
        if(pente == 0){ // pente nulle le segment n'est pas modifié
            return 0;
        }
        return newValue/pente;
    }
    if(pente == DBL_MAX){ // pente verticale le segment n'est pas modifié
        return 0;
    }
    return newValue*pente;
}
