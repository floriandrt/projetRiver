#include "statebox.h"

#include <fstream>
#include <QBrush>
#include <QPainter>
#include <QSize>
#include <QLinearGradient>
#include <QLineF>
#include <QDebug>
#include <QMessageBox>
#include <QProgressDialog>
#include "point.h"
#include <stdio.h>
#include "cair/CAIR_CML.h"
#include "cair/CAIR.h"
#include "tools.h"
#include "math.h"
#include <cfloat>

using namespace std;

/**
  *  This box can be re-sized and it can be moved. For moving, we capture
  *  the mouse move events and move the box location accordingly.
  *
  *  To resize the box, we place small indicator boxes on the four corners that give the user
  *  a visual cue to grab with the mouse. The user then drags the corner to stretch
  *  or shrink the box.  The corner grabbers are implemented with the CornerGrabber class.
  *  The CornerGrabber class captures the mouse when the mouse is over the corner's area,
  *  but the StateBox object (which owns the corners) captures and processes the mouse
  *  events on behalf of the CornerGrabbers (because the owner wants to be
  *  resized, not the CornerGrabbers themselves). This is accomplished by installed a scene event filter
  *  on the CornerGrabber objects:
          _corners[0]->installSceneEventFilter(this);
  *
  *
  *
  */

void drawImagefromData(Data& d, QImage* _img){
    QPainter qPaint;
    qPaint.begin(_img);
    qPaint.setBrush(Qt::NoBrush);
    qPaint.setPen(Qt::white);
    for(int i = 0; i<d.getDataSize(); i++){
        qPaint.drawLine(d[i][0],d[i][1],d[i][2],d[i][3]);
    }
    qPaint.end();
}

void f(Tree<Data>* t, QPainter& qPaint){
    //cerr << "ENTREE F children size : " << t->getChildrenSize() << endl;
    for(int i = 0; i<t->getChildrenSize(); i++){
        f((*t)[i], qPaint);
    }
    //cerr << "APRES REC" << endl;
    Data* temp = t->getValue();
    if(temp == NULL){
        cerr << "NULL" << endl;
    }
    for(int i = 0; i<temp->getDataSize(); i++){
        qPaint.drawLine((*temp)[i][0],(*temp)[i][1],(*temp)[i][2],(*temp)[i][3]);
    }
    //cerr << "SORTIE F" << endl;
}

void drawImagefromData(Tree<Data>& t, QImage* _img){
    //cerr << "ENTREE DRAW TREE" << endl;
    QPainter qPaint;
    qPaint.begin(_img);
    qPaint.setBrush(Qt::NoBrush);
    qPaint.setPen(Qt::white);
    for(int i = 0; i<t.getChildrenSize(); i++){
        f(t[i], qPaint);
    }
    qPaint.end();
    //cerr << "SORTIE DRAW TREE" << endl;
}

template<class T>
bool isInVector(vector<T> v, T val){
    for(int i = 0; i<(int)v.size(); i++){
       // cerr << "v[i] : " << v[i] << ", val : " << val << endl;
        if(v[i] == val){
            return true;
        }
    }
    return false;
}

Tree<Data>* findTree(Tree<Data>& t, Data* exData, vector<Tree<Data>*>& explored){
    Data* d;
    Tree<Data>* res = NULL;
    for(int i = 0; i<t.getChildrenSize(); i++){
        if(!isInVector(explored,t[i])){
            d = (t[i])->getValue();
            //cerr << "branche : " << t[i] << " | " << d << endl;
            if(d != NULL && d == exData){
                return t[i];
            }
            explored.push_back(t[i]);
        }
        res = findTree(*(t[i]),exData, explored);
        if(res != NULL){
            return res;
        }
    }
    return res;
}

void printDatTree(Tree<Data>& t, int rang, vector<Tree<Data>*>& explored){
    Data* d;
    for(int i = 0; i<t.getChildrenSize(); i++){
        if(!isInVector(explored,t[i])){
            d = (t[i])->getValue();
            if(d != NULL){
                cerr << "enfant " << rang << " branche " << i << " : " << t[i] << " | ";
                cerr << (*d)[0][0] << " " << (*d)[0][1] << " " << (*d)[0][2] << " " << (*d)[0][3] << " | ";
                cerr << (*d)[d->getDataSize()-1][0] << " " << (*d)[d->getDataSize()-1][1] << " " << (*d)[d->getDataSize()-1][2] << " " << (*d)[d->getDataSize()-1][3];
                for(int k = 0; k<(t[i])->getParentSize(); k++){
                    cerr << " --- parent : " << (t[i])->getParent(k) << " ";
                }
                cerr << endl;
            }
            explored.push_back(t[i]);
        }
        printDatTree(*(t[i]),rang+1, explored);
    }
}

void StateBox::parseFile(string filename){
    ifstream file(filename.c_str());
    if(!file){
        cerr << "fichier inexistant" << endl;
    }
    string s;
    //Titre
    getline (file,s);
    //cout << s << endl;
    //saut de ligne
    getline (file,s);
    //cout << s << endl;
    //Sample size:
    getline (file,s);
    //cout << s << endl;
    int sampleSize;
    file >> sampleSize;
    //cout << sampleSize << endl;
    //saut de ligne
    getline (file,s);
    //cout << s << endl;
    double coord;
    int nbData;
    Point p(4);
    vector<Data*> vectData;
    Data* d;
    while(s.compare(0,5,"Order") != 0){
        file >> s;
        //cout << s << endl;
        //getline (file,s);
        if(s.compare(0,9,"NewBranch") == 0){
            file >> nbData;
            //cout << "nbData : " << nbData << endl;
            d = new Data();
            vectData.push_back(d);
            for(int i = 0; i<nbData; i++){
                //x1
                file >> coord;
                //cout << coord << endl;
                p[0] = coord;
                //y1
                file >> coord;
                //cout << coord << endl;
                p[1] = coord;
                //x2
                file >> coord;
                //cout << coord << endl;
                p[2] = coord;
                //y2
                file >> coord;
                //cout << coord << endl;
                p[3] = coord;
                getline (file,s);
                //cout << s << endl;
                d->addData(p);
            }
            d->setMean(Tools::averageMulDimNorm(*d));
            d->setVar(Tools::varianceMulDimNorm(*d,d->getMean()));
            for(int i = 0; i<sampleSize; i++){
                d->addSample(Tools::generateMulDim(d->getMean(),d->getVar()));
            }
        }
    }
    int nbOrder;
    file >> nbOrder;
    //cout << nbOrder << endl;
    int parent, child;
    Tree<Data>* x;
    Tree<Data>* tempTree;
    vector<Tree<Data>*> explored;
    for(int i = 0; i<nbOrder; i++){
        file >> parent;
        file >> s;
        file >> child;
        //cerr << parent << " -> " << child << endl;
        //cerr << "avant findTree first : " << vectData[child-1] << endl;
        x = findTree(t, vectData[child-1], explored);
        explored.clear();
        //cerr << "apres findTree first" << endl;
        if(x == NULL){
            cerr << "ICI i = " << i << endl;
            x = new Tree<Data>(vectData[child-1]);
        }
        if(parent == 0){
            t.addChild(x);
        }else{
            //cerr << "parent search : " << vectData[parent-1] << endl;
            tempTree = findTree(t, vectData[parent-1], explored);
            //cerr << "fin parent search" << endl;
            explored.clear();
            tempTree->addChild(x);
        }
        //printDatTree(t,0,explored);
        explored.clear();
        //cerr << "fin for" << endl;
        getline(file,s);
        //cout << s << endl;
    }
}

//tempData doit être d1
void fuseData(Data* d1, Data* d2, bool d2first){
    (*d2)[0][0] = (*d1)[d1->getDataSize()-1][2];
    (*d2)[0][1] = (*d1)[d1->getDataSize()-1][3];
    cerr << "fusion de " << d2 << " dans " << d1 << endl;
    if(d2first){
        for(int i = 0; i<d2->getDataSize(); i++){
            d1->insert((*d2)[i],i);
        }
        cerr << "au debut" << endl;
    }else{
        for(int i = 0; i<d2->getDataSize(); i++){
            d1->addData((*d2)[i]);
        }
        cerr << "a la fin" << endl;
    }
}

//coupe une Data en deux à partir du point inter(x,y)
void separateData(vector<Data*>& tempData, Data* d, double* inter, int index){
    Data* newData = new Data();
    //s'il n'y a pas d'intersection alors le data est coupé entre 2 segments.
    if(inter == NULL){
        for(int i = index+1; i<d->getDataSize(); i++){
            newData->addData((*d)[i]);
            d->erase(i);
        }
    }else{
        Point ptGauche(4);
        ptGauche[0] = inter[0];
        ptGauche[1] = inter[1];
        ptGauche[2] = (*d)[index][2];
        ptGauche[3] = (*d)[index][3];
        (*d)[index][2] = inter[0];
        (*d)[index][3] = inter[1];
        newData->addData(ptGauche);
        for(int i = index+1; i<d->getDataSize(); i++){
            newData->addData((*d)[i]);
            d->erase(i);
        }
    }
    cerr << "separation data " << d << endl;
    tempData.push_back(newData);
}


double distance2(double x1, double y1, double x2, double y2){
    double res = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
//    if(res < 100){
//        cerr << "distance : " << res << endl;
//    }
    return res;
}

bool smallDistance(Point p1, Point p2, int** index, double threshold){
    if(distance2(p1[0],p1[1],p2[0],p2[1]) < threshold){
        (*index)[0] = 0;
        (*index)[1] = 1;
        (*index)[2] = 0;
        (*index)[3] = 1;
        return true;
    }else if(distance2(p1[0],p1[1],p2[2],p2[3]) < threshold){
        (*index)[0] = 0;
        (*index)[1] = 1;
        (*index)[2] = 2;
        (*index)[3] = 3;
        return true;
    }else if(distance2(p1[2],p1[3],p2[0],p2[1]) < threshold){
        (*index)[0] = 2;
        (*index)[1] = 3;
        (*index)[2] = 0;
        (*index)[3] = 1;
        return true;
    }else if(distance2(p1[2],p1[3],p2[2],p2[3]) < threshold){
        (*index)[0] = 2;
        (*index)[1] = 3;
        (*index)[2] = 2;
        (*index)[3] = 3;
        return true;
    }
    return false;
}

bool isNode(vector<Point> allPoints, Point p1, Point p2, double threshold, bool beginning){
    Point p;
    for(int i = 0; i<(int)allPoints.size(); i++){
        p = allPoints[i];
        if(p != p2){
            if(beginning && (distance2(p[0],p[1],p1[0],p1[1]) < threshold || distance2(p[2],p[3],p1[0],p1[1]) < threshold)){
                return true;
            }
            if(!beginning && (distance2(p[0],p[1],p1[2],p1[3]) < threshold || distance2(p[2],p[3],p1[2],p1[3]) < threshold)){
                return true;
            }
        }
    }
    return false;
}

bool isNodeInTemp(vector<Data*>& tempData, Point pCurr, Point p1, double threshold, bool beginning){
    Data* d;
    Point p;
    for(int i = 0; i<(int)tempData.size(); i++){
        d = tempData[i];
        for(int j = 0; j<d->getDataSize(); j++){
            p = (*d)[j];
            if(p != p1){
                if(beginning && (distance2(p[0],p[1],pCurr[0],pCurr[1]) < threshold || distance2(p[2],p[3],pCurr[0],pCurr[1]) < threshold)){
                    return true;
                }
                if(!beginning && (distance2(p[0],p[1],pCurr[2],pCurr[3]) < threshold || distance2(p[2],p[3],pCurr[2],pCurr[3]) < threshold)){
                    return true;
                }
            }
        }
    }
    return false;
}

double* appInter(Point seg1, Point seg2){
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
    if(a2 == a1){
        res[0] = -1;
        res[1] = -1;
        return res;
    }
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
    res[0] = resX;
    res[1] = resY;
    return res;
}

bool interInside(double* inter, Point p){
    double limMinX = p[0] < p[2] ? p[0] : p[2];
    double limMaxX = p[0] > p[2] ? p[0] : p[2];
    double limMinY = p[1] < p[3] ? p[1] : p[3];
    double limMaxY = p[1] > p[3] ? p[1] : p[3];
    if(inter[0] <= limMaxX && inter[0] >= limMinX && inter[1] <= limMaxY && inter[1] >= limMinY){
        return true;
    }
    return false;
}

void handleInter(vector<Data*>& tempData, Data* data, int index, double threshold){
    cerr << "entree handleInter" << endl;
    Data* dTemp;
    double *inter;
    Point& pCurr = (*data)[index];
    for(int i = 0; i<(int)tempData.size(); i++){
        dTemp = tempData[i];
        for(int j = 0; j<dTemp->getDataSize(); j++){
            inter = appInter(pCurr, (*dTemp)[j]);
            if(!(distance2((*dTemp)[j][0],(*dTemp)[j][1],pCurr[0],pCurr[1]) < threshold || distance2((*dTemp)[j][0],(*dTemp)[j][1],pCurr[2],pCurr[3]) < threshold ||
                    distance2((*dTemp)[j][2],(*dTemp)[j][3],pCurr[0],pCurr[1]) < threshold || distance2((*dTemp)[j][2],(*dTemp)[j][3],pCurr[2],pCurr[3]) < threshold)){
                if(distance2(inter[0],inter[1],pCurr[0],pCurr[1]) < threshold && interInside(inter,(*dTemp)[j])){
                    pCurr[0] = inter[0];
                    pCurr[1] = inter[1];
                    cerr << "intersection avec pc0 pc1 " << pCurr << endl;
                    separateData(tempData, dTemp, inter, j);
                }else if(distance2(inter[0],inter[1],pCurr[2],pCurr[3]) < threshold && interInside(inter,(*dTemp)[j])){
                    pCurr[2] = inter[0];
                    pCurr[3] = inter[1];
                    cerr << "intersection avec pc2 pc3 " << pCurr << endl;
                    separateData(tempData, dTemp, inter, j);
                }else if(distance2(inter[0],inter[1],(*dTemp)[j][0],(*dTemp)[j][1]) < threshold && interInside(inter,pCurr)){
                    (*dTemp)[j][0] = inter[0];
                    (*dTemp)[j][1] = inter[1];
                    cerr << "intersection avec p0 p1 " << pCurr << endl;
                    separateData(tempData, data, inter, index);
                }else if(distance2(inter[0],inter[1],(*dTemp)[j][2],(*dTemp)[j][3]) < threshold && interInside(inter,pCurr)){
                    (*dTemp)[j][2] = inter[0];
                    (*dTemp)[j][3] = inter[1];
                    cerr << "intersection avec p2 p3 " << pCurr << endl;
                    separateData(tempData, data, inter, index);
                }
            }
        }
    }
    cerr << "sortie handleInsert" << endl;
}

void erasePoint(vector<Point>& allPoints, Point del){
    for(int i = 0; i<(int)allPoints.size(); i++){
        if(allPoints[i] == del){
            allPoints.erase(allPoints.begin()+i);
        }
    }
}

void updateCopy(vector<Point>& copyPoints, Point p, double val1, double val2, int ind1, int ind2){
    for(int i = 0; i<(int)copyPoints.size(); i++){
        if(copyPoints[i] == p){
            copyPoints[i][ind1] = val1;
            copyPoints[i][ind2] = val2;
            return;
        }
    }
}

void recData01(vector<Point>& allPoints, vector<Point>& copyPoints, Data* dCurr, Point& pCurr, double threshold){
    Point pPot;
    bool potentialPoint = false;
    cerr << "entree recData01 " << pCurr << endl;
    for(int i = 0; i<(int)allPoints.size(); i++){
        if(allPoints[i] != pCurr && distance2(pCurr[0],pCurr[1],allPoints[i][2],allPoints[i][3]) < threshold){
            updateCopy(copyPoints,allPoints[i],pCurr[0],pCurr[1],2,3);
            allPoints[i][2] = pCurr[0];
            allPoints[i][3] = pCurr[1];
            cerr << pCurr << " " << allPoints[i] << endl;
            if(potentialPoint){
                cerr << "noeud donc pas d'ajout" << endl;
                for(int j = i+1; j<(int)allPoints.size(); j++){
                    if(allPoints[j] != pCurr && distance2(pCurr[0],pCurr[1],allPoints[j][2],allPoints[j][3]) < threshold){
                        updateCopy(copyPoints,allPoints[i],pCurr[0],pCurr[1],2,3);
                        allPoints[j][2] = pCurr[0];
                        allPoints[j][3] = pCurr[1];
                    }else if(allPoints[j] != pCurr && distance2(pCurr[0],pCurr[1],allPoints[j][0],allPoints[j][1]) < threshold){
                        updateCopy(copyPoints,allPoints[i],pCurr[0],pCurr[1],0,1);
                        allPoints[j][0] = pCurr[0];
                        allPoints[j][1] = pCurr[1];
                    }
                }
                return;
            }
            potentialPoint = true;
            pPot = allPoints[i];
        }else if(allPoints[i] != pCurr && distance2(pCurr[0],pCurr[1],allPoints[i][0],allPoints[i][1]) < threshold){
            updateCopy(copyPoints,allPoints[i],pCurr[0],pCurr[1],0,1);
            allPoints[i][0] = pCurr[0];
            allPoints[i][1] = pCurr[1];
            cerr << pCurr << " " << allPoints[i] << endl;
            if(potentialPoint){
                cerr << "noeud donc pas d'ajout" << endl;
                for(int j = i+1; j<(int)allPoints.size(); j++){
                    if(allPoints[j] != pCurr && distance2(pCurr[0],pCurr[1],allPoints[j][2],allPoints[j][3]) < threshold){
                        updateCopy(copyPoints,allPoints[j],pCurr[0],pCurr[1],2,3);
                        allPoints[j][2] = pCurr[0];
                        allPoints[j][3] = pCurr[1];
                    }else if(allPoints[j] != pCurr && distance2(pCurr[0],pCurr[1],allPoints[j][0],allPoints[j][1]) < threshold){
                        updateCopy(copyPoints,allPoints[j],pCurr[0],pCurr[1],0,1);
                        allPoints[j][0] = pCurr[0];
                        allPoints[j][1] = pCurr[1];
                    }
                }
                return;
            }
            potentialPoint = true;
            pPot = allPoints[i];
        }
    }
    if(potentialPoint){
        cerr << "ajout point " << pPot << endl;
        dCurr->insert(pPot,0);
        erasePoint(copyPoints,pPot);
        recData01(allPoints,copyPoints,dCurr,(*dCurr)[0],threshold);
    }
}

void recData23(vector<Point>& allPoints, vector<Point>& copyPoints, Data* dCurr, Point& pCurr, double threshold){
    cerr << "entree recData23 " << pCurr << endl;
    Point pPot;
    Point p;
    bool potentialPoint = false;
    for(int i = 0; i<(int)allPoints.size(); i++){
//        cerr << "point de comparaison : " << allPoints[i] << endl;
        p = allPoints[i];
        if(allPoints[i] != pCurr && distance2(pCurr[2],pCurr[3],allPoints[i][2],allPoints[i][3]) < threshold){
            updateCopy(copyPoints,allPoints[i],pCurr[2],pCurr[3],2,3);
            (allPoints[i])[2] = pCurr[2];
            (allPoints[i])[3] = pCurr[3];
            cerr << pCurr << " " << allPoints[i] << endl;
            if(potentialPoint){
                cerr << "noeud donc pas d'ajout" << endl;
                for(int j = i+1; j<(int)allPoints.size(); j++){
                    if(allPoints[j] != pCurr && distance2(pCurr[2],pCurr[3],allPoints[j][2],allPoints[j][3]) < threshold){
                        updateCopy(copyPoints,allPoints[j],pCurr[2],pCurr[3],2,3);
                        allPoints[j][2] = pCurr[2];
                        allPoints[j][3] = pCurr[3];
                    }else if(allPoints[j] != pCurr && distance2(pCurr[2],pCurr[3],allPoints[j][0],allPoints[j][1]) < threshold){
                        updateCopy(copyPoints,allPoints[j],pCurr[2],pCurr[3],0,1);
                        allPoints[j][0] = pCurr[2];
                        allPoints[j][1] = pCurr[3];
                    }
                }
                return;
            }
            potentialPoint = true;
            pPot = allPoints[i];
        }else if(allPoints[i] != pCurr && distance2(pCurr[2],pCurr[3],allPoints[i][0],allPoints[i][1]) < threshold){
            updateCopy(copyPoints,allPoints[i],pCurr[2],pCurr[3],0,1);
            (allPoints[i])[0] = pCurr[2];
            (allPoints[i])[1] = pCurr[3];
            cerr << pCurr << " " << allPoints[i] << endl;
            if(potentialPoint){
                cerr << "noeud donc pas d'ajout" << endl;
                for(int j = i+1; j<(int)allPoints.size(); j++){
                    if(allPoints[j] != pCurr && distance2(pCurr[2],pCurr[3],allPoints[j][2],allPoints[j][3]) < threshold){
                        updateCopy(copyPoints,allPoints[j],pCurr[2],pCurr[3],2,3);
                        allPoints[j][2] = pCurr[2];
                        allPoints[j][3] = pCurr[3];
                    }else if(allPoints[j] != pCurr && distance2(pCurr[2],pCurr[3],allPoints[j][0],allPoints[j][1]) < threshold){
                        updateCopy(copyPoints,allPoints[j],pCurr[2],pCurr[3],0,1);
                        allPoints[j][0] = pCurr[2];
                        allPoints[j][1] = pCurr[3];
                    }
                }
                return;
            }
            potentialPoint = true;
            pPot = allPoints[i];
        }
    }
    if(potentialPoint){
        cerr << "ajout point " << pPot << endl;
        dCurr->addData(pPot);
        erasePoint(copyPoints,pPot);
        recData23(allPoints,copyPoints,dCurr,(*dCurr)[dCurr->getDataSize()-1],threshold);
    }
}

void buildData(vector<Data*>& tempData, vector<Point>& allPoints, double& width, double& height){
    vector<Point> copyPoints;
    for(int i = 0; i<(int)allPoints.size(); i++){
        copyPoints.push_back(allPoints[i]);
    }
    double threshold = 200;
    Data* newData;
    Point pCurr;
    cerr << "copyPoints size : " << copyPoints.size() << endl;
    while(copyPoints.size()>0){
        cerr << "you spin me right round : " << copyPoints.size() << endl;
        cerr << "nouvel data avec " << copyPoints[0] << endl;
        newData = new Data();
        pCurr = copyPoints[0];
        newData->addData(pCurr);
        tempData.push_back(newData);
        copyPoints.erase(copyPoints.begin());
        recData01(allPoints,copyPoints,newData,pCurr,threshold);
        cerr << "progression" << endl;
        recData23(allPoints,copyPoints,newData,pCurr,threshold);
    }
//    for(int k = 0; k<(int)tempData.size(); k++){
//        cerr << "data : " << k << endl;
//        for(int j = 0; j<(tempData[k])->getDataSize(); j++){
//            cerr << (*(tempData[k]))[j][0] << " " << (*(tempData[k]))[j][1] << " " << (*(tempData[k]))[j][2] << " " << (*(tempData[k]))[j][3] << endl;
//        }
//    }
    for(int i = 0; i<(int)tempData.size(); i++){
        for(int j = 0; j<(tempData[i])->getDataSize(); j++){
            handleInter(tempData,tempData[i],j,threshold);
        }
    }
    Data* d;
    int sampleSize = 1000;
    double minX, minY, maxX, maxY;
    minX = minY = 500;
    maxX = maxY = 0;
    for(int k = 0; k<(int)tempData.size(); k++){
        cerr << "data : " << k << endl;
        d = tempData[k];
        d->setMean(Tools::averageMulDimNorm(*d));
        d->setVar(Tools::varianceMulDimNorm(*d,d->getMean()));
        for(int i = 0; i<sampleSize; i++){
            d->addSample(Tools::generateMulDim(d->getMean(),d->getVar()));
        }
        for(int j = 0; j<(tempData[k])->getDataSize(); j++){
            minX = minX > (*d)[j][0] ? (*d)[j][0] : minX;
            minY = minY > (*d)[j][1] ? (*d)[j][1] : minY;
            minY = minY > (*d)[j][3] ? (*d)[j][3] : minY;
            maxX = maxX < (*d)[j][2] ? (*d)[j][2] : maxX;
            maxY = maxY < (*d)[j][1] ? (*d)[j][1] : maxY;
            maxY = maxY < (*d)[j][3] ? (*d)[j][3] : maxY;
            cerr << (*(tempData[k]))[j][0] << " " << (*(tempData[k]))[j][1] << " " << (*(tempData[k]))[j][2] << " " << (*(tempData[k]))[j][3] << endl;
        }
    }
    for(int i = 0; i<(int)tempData.size(); i++){
        d = tempData[i];
        for(int j = 0; j<d->getDataSize(); j++){
            (*d)[j][0] -= minX;
//            (*d)[j][1] -= minY;
            (*d)[j][2] -= minX;
//            (*d)[j][3] -= minY;
        }
    }
    width = maxX - minX;
    height = 500; //maxY - minY;
}

void createTree(vector<Data*> tempData, Tree<Data>& root){
    cerr << "entree createTree : " << tempData.size() << endl;
    Data* dCurr;
    Data* d;
    vector<Tree<Data>*> piecesofTree;
    for(int i = 0; i<(int)tempData.size(); i++){
        piecesofTree.push_back(new Tree<Data>(tempData[i]));
    }
    cerr << "taille du vector tree " << piecesofTree.size() << endl;
    for(int i = 0; i<(int)tempData.size(); i++){
        dCurr = tempData[i];
        int dcFin = dCurr->getDataSize()-1;
        bool hasParents = false;
        for(int j = 0; j<(int)tempData.size(); j++){
            d = tempData[j];
            int dFin = d->getDataSize()-1;
            if(d != dCurr){
                if((*dCurr)[0][0] == (*d)[dFin][2] && (*dCurr)[0][1] == (*d)[dFin][3]){
                    (piecesofTree[i])->addParent(piecesofTree[j]);
                    hasParents = true;
                }
                if((*dCurr)[dcFin][2] == (*d)[0][0] && (*dCurr)[dcFin][3] == (*d)[0][1]){
                    (piecesofTree[i])->addChild(piecesofTree[j]);
                }
            }
        }
        if(!hasParents){
            root.addChild(piecesofTree[i]);
        }
    }

    //affichage
    vector<Tree<Data>*> explored;
    cerr << "root : " << &root << endl;
    printDatTree(root, 0, explored);
    cerr << "sortie createTree" << endl;
}

StateBox::StateBox(ImageScene *_scene, QImage *_energy, Ui::ResizeWidget* wid, string filename, vector<QLine> _paintedLines):
        _outterborderColor(Qt::black),
        _outterborderPen(),
        _location(0,0),
        _dragStart(0,0),
        _gridSpace(10),
        _cornerDragStart(0,0),
        _XcornerGrabBuffer(0),
        _YcornerGrabBuffer(0),
        _drawingOrigenX(10),
        _drawingOrigenY(10),
        _mainScene(_scene),
        editionModeNotChecked(true),
        _resizeWidget(wid),
        _img_energy(_energy)
{
    cerr << "ENTREE INIT STATEBOX" << endl;
    _img_energy->fill(QColor(0,0,0));

    /*
    Data* d1 = new Data();
    Data* d2 = new Data();
    Data* d3 = new Data();
    Data* d4 = new Data();
    Data* d5 = new Data();
    Data* d6 = new Data();
    int sampleSize = 1000;
    Point p11(4);
    p11[0] = 0;
    p11[1] = 110;
    p11[2] = 50;
    p11[3] = 190;
    d1->addData(p11);
    Point p12(4);
    p12[0] = 50;
    p12[1] = 190;
    p12[2] = 100;
    p12[3] = 160;
    d1->addData(p12);
    d1->setMean(Tools::averageMulDimNorm(*d1));
    d1->setVar(Tools::varianceMulDimNorm(*d1,d1->getMean()));
    for(int i = 0; i<sampleSize; i++){
        d1->addSample(Tools::generateMulDim(d1->getMean(),d1->getVar()));
    }

    cerr << "enfant -1 branche 0 : " << &t << endl;
    Tree<Data>* x = new Tree<Data>(d1);
    t.addChild(x);
    cerr << "enfant 0 branche 1 : " << x << endl;
    */
    int sampleSize = 10000;
    vector<Data*> tempData;
    vector<Point> allPoints;
    Point pCurr(4);
    cerr << "affichage paintedLines" << endl;
    for(int i = 0; i<(int)_paintedLines.size(); i++){
        pCurr[0] = (_paintedLines[i]).x1();
        pCurr[1] = (_paintedLines[i]).y1();
        pCurr[2] = (_paintedLines[i]).x2();
        pCurr[3] = (_paintedLines[i]).y2();
        allPoints.push_back(pCurr);
        cerr << pCurr[0] << " " << pCurr[1] << " " << pCurr[2] << " " << pCurr[3] << endl;
    }
    cerr << "fin affichage paintedLines" << endl;
    for(int i = 0; i<(int)allPoints.size(); i++){
        cerr << allPoints[i] << endl;
    }
    double imgWidth, imgHeight;
    buildData(tempData,allPoints,imgWidth,imgHeight);
//    pCurr[0] = (_paintedLines[0]).x1();
//    pCurr[1] = (_paintedLines[0]).y1();
//    pCurr[2] = (_paintedLines[0]).x2();
//    pCurr[3] = (_paintedLines[0]).y2();
//    cerr << "insertion du premier point : " << pCurr[0] << " " << pCurr[1] << " " << pCurr[2] << " " << pCurr[3] << endl;
//    firstData->addData(pCurr);
//    tempData.push_back(firstData);
//    for(int i = 1; i<(int)_paintedLines.size(); i++){
//        pCurr[0] = (_paintedLines[i]).x1();
//        pCurr[1] = (_paintedLines[i]).y1();
//        pCurr[2] = (_paintedLines[i]).x2();
//        pCurr[3] = (_paintedLines[i]).y2();
//        cerr << pCurr[0] << " " << pCurr[1] << " " << pCurr[2] << " " << pCurr[3] << endl;
//        insertOrCreate(allPoints, tempData, pCurr);
//    }
    Data *d;
//    for(int i = 0; i<(int)tempData.size(); i++){
//        cerr << "data : " << i << endl;
//        d = tempData[i];
//        d->setMean(Tools::averageMulDimNorm(*d));
//        d->setVar(Tools::varianceMulDimNorm(*d,d->getMean()));
//        for(int i = 0; i<sampleSize; i++){
//            d->addSample(Tools::generateMulDim(d->getMean(),d->getVar()));
//        }
//        for(int j = 0; j<(tempData[i])->getDataSize(); j++){
//            cerr << (*(tempData[i]))[j][0] << " " << (*(tempData[i]))[j][1] << " " << (*(tempData[i]))[j][2] << " " << (*(tempData[i]))[j][3] << endl;
//        }
//    }
    cerr << "fin boucle insert" << endl;
    createTree(tempData, t);
    cerr << "fin create" << endl;
    //parseFile("config.flo");
    vector<Tree<Data>*> explored;
    printDatTree(t, 0, explored);

    _outterborderPen.setWidth(2);
    _outterborderPen.setColor(_outterborderColor);

    //QString filename("C:/Users/Florian/Pictures/baby_sun.jpg");
    //_img = new QImage(filename);
    _img = new QImage(QSize(imgWidth,imgHeight),QImage::Format_RGB32);
    drawImagefromData(t,_img);
    _img_view = _img;

    _width = _img->width()+32;
    _height = _img->height()+32;

    _drawingWidth = _width -16 -   _XcornerGrabBuffer;
    _drawingHeight = _height -16 -   _XcornerGrabBuffer;

    this->setAcceptHoverEvents(true);
    cerr << "SORTIE INIT STATEBOX" << endl;
}

//assumes dest is already set to have the save size as source
static void QImagetoCML(QImage source, CML_color &dest)
{
  CML_RGBA p;
  for( int j=0; j<source.height(); j++ )
  {
    for( int i=0; i<source.width(); i++ )
    {
      p.red = qRed( source.pixel(i, j) );
      p.green = qGreen( source.pixel(i, j) );
      p.blue = qBlue( source.pixel(i, j) );
      p.alpha = qAlpha( source.pixel(i, j) );
      dest(i,j) = p;
    }
  }
}

static QImage CMLtoQImage(CML_color &source)
{
  CML_RGBA p;
  QImage newImg = QImage(source.Width(), source.Height(), QImage::Format_RGB32);
  for( int j=0; j<source.Height(); j++ )
  {
    for( int i=0; i<source.Width(); i++ )
    {
      p = source(i,j);
      newImg.setPixel(i, j, qRgba( p.red, p.green, p.blue, p.alpha ));
    }
  }
  return newImg;
}

bool updateCallbackStateBox(float percDone)
{
//  qApp->processEvents();
//  gProg->setValue((int)(percDone*100));
//  if(gProg->wasCanceled())
//    return false; //exit out
  return true;
}

/**
 *  To allow the user to grab the corners to re-size, we need to get a hover
 *  indication. But if the mouse pointer points to the left, then when the mouse
 *  tip is to the left but just outsize the box, we will not get the hover.
 *  So the solution is to tell the graphics scene the box is larger than
 *  what the painter actually paints in. This way when the user gets the mouse
 *  within a few pixels of what appears to be the edge of the box, we get
 *  the hover indication.

 *  So the cornerGrabBuffer is a few pixel wide buffer zone around the outside
 *  edge of the box.
 *
 */
void StateBox::adjustSize(int x, int y)
{
    cerr << "ajustSize entree" << endl;
      int width = _img->width();
      int height = _img->height();
      _width += x;
      _height += y;
      int newWidth = width+x;
      int newHeight = height+y;
      _drawingWidth += x;
      _drawingHeight += y;

      int weight_scale = (int)(_resizeWidget->weightScaleLineEdit->text().toInt() * (_resizeWidget->brushWeightSlider->value() / 100.0));
      cerr << "apres weight scale" << endl;
      CAIR_convolution conv = LAPLACIAN_LOG; //V1;
      switch( _resizeWidget->edgeDetector->currentIndex() )
      {
        case 0 : conv = V1; break;
        case 1 : conv = V_SQUARE; break;
        case 2 : conv = PREWITT; break;
        case 3 : conv = SOBEL; break;
        case 4 : conv = LAPLACIAN; break;
        case 5 : conv = LAPLACIAN_LOG; break;
      }

      CAIR_energy ener = BACKWARD;
//      if( _resizeWidget.energyCheckBox->isChecked() )
//      {
//        ener = FORWARD;
//      }

      if(newWidth < 1 || newHeight < 1)
      {
//        QMessageBox::information(this, tr("Seam Carving GUI"),
//                                 tr("Invalid dimensions."));
        return;
      }
      if(width == newWidth && height == newHeight)
        return;

      //int widthAdjustment = (width < newWidth ? newWidth - width : width - newWidth);
      //int heightAdjustment = (height < newHeight ? newHeight - height : height - newHeight);
//      QProgressDialog prog("Resizing...", "&Cancel", 0, 100, this);
//      gProg = &prog;
//      qApp->processEvents();
      cerr << "avant init mskImg" << endl;
      //Transfer the image over to cair image format.
      CML_color source(width, height);
//      CML_color dest(1, 1);
      CML_int source_weights(width, height);
      CML_int dest_weights(1,1);
      QImagetoCML(*_img,source);
//      QImage mskImg = _maskPix->toImage();
      cerr << "avant init mask" << endl;
      for( int j=0; j<height; j++ )
      {
        for( int i=0; i<width; i++ )
        {
//          QRgb m = mskImg.pixel(i,j);
//          if(qGreen(m) > 0)
//            source_weights(i,j) = (int)(qGreen(m) * weight_scale);
//          else if(qRed(m) > 0)
//            source_weights(i,j) = (int)(qRed(m) * -weight_scale);
//          else
            source_weights(i,j) = 0;
        }
      }
      //Call CAIR
      int newLength = newWidth;
      bool changeHeight = false;
      if(height != newHeight){
          newLength = newHeight;
          changeHeight = true;
      }
      cerr << "avant cair" << endl;
      CAIR_Data(&source, &source_weights, &dest_weights, newLength, changeHeight, conv, ener, t, _img_energy, &min_path);
      cerr << "apres cair" << endl;
//      if( !_resizeWidget.hdCheckBox->isChecked() )
//      {
//        CAIR( &source, &source_weights, newWidth, newHeight, conv, ener, &dest_weights, &dest, updateCallbackStateBox );
        //      }
//      else
//      {
//        CAIR_HD( &source, &source_weights, newWidth, newHeight, conv, ener, &dest_weights, &dest, updateCallback );
//      }
//      if(prog.wasCanceled())
//        return;
      //QImage *newImg = new QImage(CMLtoQImage(dest));
//      saveInUndoStack();
      cerr << "avant delete img" << endl;
      delete _img;
      _img = new QImage(QSize(newWidth,newHeight),QImage::Format_RGB32);
      _img->fill(0);
      drawImagefromData(t,_img);
      cerr << "apres draw" << endl;
      _img_view = _img;
      /*
      delete _img_energy;
      _img_energy = new QImage(QSize(newWidth,newHeight),QImage::Format_RGB32);
      _img_energy->fill(0);
      drawEnergyMap(energyMap,_img_energy);
      */

//      _img = new QImage(CMLtoQImage(dest));
      //_imgItem->setPixmap(QPixmap::fromImage(*_img));
      //Set the weight mask to the now reduced size version shrunk by CAIR
      //cerr << "AVANT DEST->MASK" << endl;


//      QImage maskImg(newWidth, newHeight, QImage::Format_ARGB32);
//      maskImg.fill(Qt::transparent);
//      for( int j=0; j<newHeight; j++ )
//      {
//        for( int i=0; i<newWidth; i++ )
//        {
//            if(qGreen(mskImg.pixel(i,j)) != 0){
//                cerr << "(" << i << "," << j << ") : " << qGreen(mskImg.pixel(i,j)) << endl;
//            }
//          if(dest_weights(i,j) > 0){
//            maskImg.setPixel(i, j, qRgba(0,255,0,0));
//            maskImg.setPixel(i, j, qRgba( 0, qGreen(mskImg.pixel(i,j)) /*dest_weights(i,j) / weight_scale*/, 0, 0/*dest_weights(i,j) / weight_scale*/));
//          }else if(dest_weights(i,j) < 0){
//            maskImg.setPixel(i, j, qRgba( dest_weights(i,j) / -weight_scale, 0, 0, dest_weights(i,j) / -weight_scale));
//          }
//        }
//      }
//      for( int j=0; j<height; j++ )
//      {
//        for( int i=0; i<width; i++ )
//        {
//            if(qGreen(maskImg.pixel(i,j)) != 0){
//                cerr << "(" << i << "," << j << ") : " << qGreen(maskImg.pixel(i,j)) << endl;
//            }
//        }
//      }

//      delete _maskPix;
//      _maskPix = new QPixmap(QPixmap::fromImage(maskImg));
//      _maskItem->setPixmap(*_maskPix);

    cerr << "avant update" << endl;
      update();
      cerr << "fin adjustSize" << endl;
}

void StateBox::changeView(QImage _image){
    if(_img_view != _img){
        delete _img_view;
    }
    _img_view = new QImage(_image);
}

QImage* StateBox::getImage(){
    return _img;
}

void StateBox::setImage(){
    if(_img_view != _img){
        delete _img_view;
    }
    _img_view = _img;
}

ImageScene* StateBox::getScene(){
    return _mainScene;
}

void StateBox::setEditionBool(bool b){
    editionModeNotChecked = b;
}

void StateBox::setMaskPix(QPixmap *mskP){
    _maskPix = mskP;
}

void StateBox::setMaskItem(QGraphicsPixmapItem *mskI){
    _maskItem = mskI;
}

/**
  * This scene event filter has been registered with all four corner grabber items.
  * When called, a pointer to the sending item is provided along with a generic
  * event.  A dynamic_cast is used to determine if the event type is one of the events
  * we are interrested in.
  */
bool StateBox::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
    //qDebug() << " QEvent == " + QString::number(event->type());
    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }


    switch (event->type() )
    {
            // if the mouse went down, record the x,y coords of the press, record it inside the corner object
        case QEvent::GraphicsSceneMousePress:
            {
                corner->setMouseState(CornerGrabber::kMouseDown);
                corner->mouseDownX = mevent->pos().x();
                corner->mouseDownY = mevent->pos().y();
            }
            break;

        case QEvent::GraphicsSceneMouseRelease:
            {
                corner->setMouseState(CornerGrabber::kMouseReleased);

            }
            break;

        case QEvent::GraphicsSceneMouseMove:
            {
                corner->setMouseState(CornerGrabber::kMouseMoving );
            }
            break;

        default:
            // we dont care about the rest of the events
            return false;
            break;
    }


    if ( corner->getMouseState() == CornerGrabber::kMouseMoving )
    {

        qreal x = mevent->pos().x(), y = mevent->pos().y();

        // depending on which corner has been grabbed, we want to move the position
        // of the item as it grows/shrinks accordingly. so we need to eitehr add
        // or subtract the offsets based on which corner this is.

        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( corner->getCorner() )
        {
        case 0:
            {
                XaxisSign = +1;
                YaxisSign = +1;
            }
            break;

        case 1:
            {
                XaxisSign = -1;
                YaxisSign = +1;
            }
            break;

        case 2:
            {
                XaxisSign = -1;
                YaxisSign = -1;
            }
            break;

        case 3:
            {
                XaxisSign = +1;
                YaxisSign = -1;
            }
            break;

        case 4:
            {
                XaxisSign = +1;
                YaxisSign = 0;
            }
            break;

        case 5:
            {
                XaxisSign = -1;
                YaxisSign = 0;
            }
            break;

        case 6:
            {
                XaxisSign = 0;
                YaxisSign = +1;
            }
            break;

        case 7:
            {
                XaxisSign = 0;
                YaxisSign = -1;
            }
            break;

        }

        // if the mouse is being dragged, calculate a new size and also re-position
        // the box to give the appearance of dragging the corner out/in to resize the box

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        int newWidth = _width + ( XaxisSign * xMoved);
        if ( newWidth < 40 ) newWidth  = 40;

        int newHeight = _height + (YaxisSign * yMoved) ;
        if ( newHeight < 40 ) newHeight = 40;

        int deltaWidth  =   newWidth - _width;
        int deltaHeight =   newHeight - _height;

        adjustSize(  deltaWidth ,   deltaHeight);

        deltaWidth *= (-1);
        deltaHeight *= (-1);

        if ( corner->getCorner() == 0 )
        {
            int newXpos = this->pos().x() + deltaWidth;
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(newXpos, newYpos);
        }
        else   if ( corner->getCorner() == 1 || corner->getCorner() == 6)
        {
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(this->pos().x(), newYpos);
        }
        else   if ( corner->getCorner() == 3 || corner->getCorner() == 4)
        {
            int newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos,this->pos().y());
        }
        else   if ( corner->getCorner() == 4 )
        {
            int newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos,this->pos().y());
        }
        setCornerPositions();

        this->update();
    }
    return true;// true => do not send event to watched - we are finished with this event
}



// for supporting moving the box across the scene
void StateBox::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    if(editionModeNotChecked){
        event->setAccepted(true);
        _location.setX( ( static_cast<int>(_location.x()) / _gridSpace) * _gridSpace );
        _location.setY( ( static_cast<int>(_location.y()) / _gridSpace) * _gridSpace );
        this->setPos(_location);
    }
}


// for supporting moving the box across the scene
void StateBox::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    if(editionModeNotChecked){
        event->setAccepted(true);
        _dragStart = event->pos();
    }
}


// for supporting moving the box across the scene
void StateBox::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    if(editionModeNotChecked){
        QPointF newPos = event->pos() ;
        _location += (newPos - _dragStart);
        this->setPos(_location);
    }
}

// remove the corner grabbers

void StateBox::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
    if(editionModeNotChecked){
        _outterborderColor = Qt::black;

        _corners[0]->setParentItem(NULL);
        _corners[1]->setParentItem(NULL);
        _corners[2]->setParentItem(NULL);
        _corners[3]->setParentItem(NULL);
        _corners[4]->setParentItem(NULL);
        _corners[5]->setParentItem(NULL);
        _corners[6]->setParentItem(NULL);
        _corners[7]->setParentItem(NULL);

        delete _corners[0];
        delete _corners[1];
        delete _corners[2];
        delete _corners[3];
        delete _corners[4];
        delete _corners[5];
        delete _corners[6];
        delete _corners[7];
    }
}


// create the corner grabbers

void StateBox::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    if(editionModeNotChecked){
        _outterborderColor = Qt::red;

        _corners[0] = new CornerGrabber(this,0);
        _corners[1] = new CornerGrabber(this,1);
        _corners[2] = new CornerGrabber(this,2);
        _corners[3] = new CornerGrabber(this,3);
        _corners[4] = new CornerGrabber(this,4);
        _corners[5] = new CornerGrabber(this,5);
        _corners[6] = new CornerGrabber(this,6);
        _corners[7] = new CornerGrabber(this,7);


        _corners[0]->installSceneEventFilter(this);
        _corners[1]->installSceneEventFilter(this);
        _corners[2]->installSceneEventFilter(this);
        _corners[3]->installSceneEventFilter(this);
        _corners[4]->installSceneEventFilter(this);
        _corners[5]->installSceneEventFilter(this);
        _corners[6]->installSceneEventFilter(this);
        _corners[7]->installSceneEventFilter(this);


        setCornerPositions();
    }

}

void StateBox::setCornerPositions()
{
    _corners[0]->setPos(_drawingOrigenX, _drawingOrigenY);
    _corners[1]->setPos(_drawingWidth,  _drawingOrigenY);
    _corners[2]->setPos(_drawingWidth , _drawingHeight);
    _corners[3]->setPos(_drawingOrigenX,_drawingHeight);
    _corners[4]->setPos(_drawingOrigenX, _drawingOrigenY+((_drawingOrigenY+_drawingHeight)/2-3));
    _corners[5]->setPos(_drawingWidth, _drawingOrigenY+((_drawingOrigenY+_drawingHeight)/2)-3);
    _corners[6]->setPos(_drawingOrigenX+((_drawingOrigenX+_drawingWidth)/2)-3 , _drawingOrigenY);
    _corners[7]->setPos(_drawingOrigenX+((_drawingOrigenX+_drawingWidth)/2)-3,_drawingHeight);
}

QRectF StateBox::boundingRect() const
{
    return QRectF(0,0,_width,_height);
}


// example of a drop shadow effect on a box, using QLinearGradient and two boxes

void StateBox::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    /*
    if(_img != 0){
    */
        QRectF target(16.0, 16.0, _img_view->width(), _img_view->height());
        QRectF source(0.0, 0.0, _img_view->width(), _img_view->height());


        painter->drawImage(target, *_img_view, source);

        QRectF target_energy(16.0, 526.0, _img_view->width(), _img_view->height());
        QRectF source_energy(0.0, 0.0, _img_view->width(), _img_view->height());
        //painter->drawImage(target_energy, *_img_energy, source_energy);
     /*
    }
    */
//    for(int i = 0; i<points.getDataSize(); i++){
//        painter->drawLine(points[i][0],points[i][1],points[i][2],points[i][3]);
//        painter->drawLine(test,test,test,test);
//    }
    _mainScene->update();
}


void StateBox::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(editionModeNotChecked){
        event->setAccepted(false);
    }
}

void StateBox::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    if(editionModeNotChecked){
        event->setAccepted(false);
    }
}

