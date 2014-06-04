#ifndef TREE_H
#define TREE_H

#include <cstddef>
#include <iostream>
#include <stdlib.h>
#include <vector>

template<class T>
class Tree{
private:
    bool changeY; //vrai si on modifie l'image sur la hauteur
    T* value; //pointeur sur ce que contient un noeud
    std::vector<Tree*> children; //tableau de pointeurs des enfants de ce noeud
    std::vector<Tree*> parents; //tableau de pointeurs des parents de ce noeud
    std::vector<Tree<T>*> recentDataTreeX; //tableau des données en cours de modification
    std::vector<Tree<T>*> recentDataTreeY; //tableau des données en cours de modification

public:
    Tree() : changeY(false), value(NULL){}
    Tree(T* v);
    Tree(const Tree& t);
    ~Tree();
    bool testGenerateNode();
    void setValue(T* v);
    T* getValue();
    void setChangeY(bool b);
    bool getChangeY();
    void addParent(Tree<T>* p);
    Tree<T>* getParent(int i);
    int getParentSize();
    void setParentsVec(std::vector<Tree<T>*> p);
    std::vector<Tree<T>*>& getParentsVec();
    void clearParents();
    void addChild(Tree<T>* c);
    int getChildrenSize();
    void setChildrenVec(std::vector<Tree<T>*> c);
    void replaceChildren(std::vector<Tree<T>*> c, Tree<T>* remChild);
    void eraseChild(Tree<T>* remChild);
    std::vector<Tree<T>*>& getChildrenVec();
    void clearChildren();
    void addRecentDataTree(Tree<T>* t);
    void eraseRecentDataTree(Tree<T>* t);
    int getRecentTreeSize();
    void clearRecent();
    Tree<T>* getRecentDataTree(int i);
    std::vector<Tree<T>*>& getRecentVecTree();
    void setRecentVec(std::vector<Tree<T>*> r);
    int getRecentSize();
    bool notInRecentTree(Tree<T>* t);
    void insertNode(T* firstHalf, T* secondHalf, std::vector<T*> datas, int nbNode, int connNode);
    Tree<T>* operator[](int ind);
    template<class S>
    friend std::ostream& operator<<(std::ostream& out, const Tree<S>& t);
};

template<class T>
Tree<T>::Tree(T* v){
    changeY = false;
    value = v;
}

template<class T>
Tree<T>::Tree(const Tree& t){
    changeY = t.changeY;
    value = new T(*(t.value));
    children = t.children;
    parents = t.parents;
    recentDataTreeX = t.recentDataTreeX;
    recentDataTreeY = t.recentDataTreeY;
}

template<class T>
Tree<T>::~Tree(){
    std::cerr << "destruction d'arbre, value : " << *value << std::endl;
    delete value;
    for(int i = 0; i<(int)children.size(); i++){
        delete(children[i]);
    }
}

template<class T>
bool Tree<T>::testGenerateNode(){
//    if((rand()%10)==0){
//        return true;
//    }
    return false;
}

template<class T>
void Tree<T>::setValue(T *v){
    delete value;
    value = v;
}

template<class T>
T* Tree<T>::getValue(){
    return value;
}

template<class T>
void Tree<T>::setChangeY(bool b){
    changeY = b;
}

template<class T>
bool Tree<T>::getChangeY(){
    return changeY;
}

template<class T>
void Tree<T>::addParent(Tree<T>* p){
    for(int i = 0; i<(int)parents.size(); i++){
        if(parents[i] == p){
            return;
        }
    }
    parents.push_back(p);
}

template<class T>
Tree<T>* Tree<T>::getParent(int i){
    return parents[i];
}

template<class T>
int Tree<T>::getParentSize(){
    return parents.size();
}

template<class T>
void Tree<T>::setParentsVec(std::vector<Tree<T>*> p){
    parents = p;
}

template<class T>
std::vector<Tree<T>*>& Tree<T>::getParentsVec(){
    return parents;
}

template<class T>
void Tree<T>::clearParents(){
    parents.clear();
}

template<class T>
void Tree<T>::addChild(Tree<T>* c){
    for(int i = 0; i<(int)children.size(); i++){
        if(children[i] == c){
            return;
        }
    }
    children.push_back(c);
    children[children.size()-1]->addParent(this);
}

template<class T>
int Tree<T>::getChildrenSize(){
    return children.size();
}

template<class T>
void Tree<T>::setChildrenVec(std::vector<Tree<T>*> c){
    children = c;
}

template<class T>
void Tree<T>::replaceChildren(std::vector<Tree<T>*> c, Tree<T>* remChild){
    for(int i = 0; i<(int)children.size(); i++){
        if(children[i] == remChild){
            children.erase(children.begin()+i);
            for(int j = 0; j<(int)c.size(); j++){
                children.push_back(c[j]);
            }
            return;
        }
    }
}

template<class T>
void Tree<T>::eraseChild(Tree<T>* remChild){
    for(int i = 0; i<(int)children.size(); i++){
        if(children[i] == remChild){
            children.erase(children.begin()+i);
            return;
        }
    }
}

template<class T>
std::vector<Tree<T>*>& Tree<T>::getChildrenVec(){
    return children;
}

template<class T>
void Tree<T>::clearChildren(){
    children.clear();
}

template<class T>
void Tree<T>::addRecentDataTree(Tree<T>* t){
    if(changeY){
        recentDataTreeY.push_back(t);
    }else{
        recentDataTreeX.push_back(t);
    }
}

template<class T>
void Tree<T>::eraseRecentDataTree(Tree<T>* t){
    std::vector<Tree<T>*>& recentDataTree = recentDataTreeX;
    if(changeY){
        recentDataTree = recentDataTreeY;
    }
    for(int i = 0; i<(int)recentDataTree.size(); i++){
        std::cerr << "ERASE RECENT t : " << t << ", recent i : " << recentDataTree[i] << std::endl;
        if(t == recentDataTree[i]){
            recentDataTree.erase(recentDataTree.begin()+i);
            return;
        }
    }
}

template<class T>
Tree<T>* Tree<T>::getRecentDataTree(int i){
    if(changeY){
        return recentDataTreeY[i];
    }else{
        return recentDataTreeX[i];
    }
}

template<class T>
void Tree<T>::clearRecent(){
    if(changeY){
        recentDataTreeY.clear();
    }else{
        recentDataTreeX.clear();
    }
}

template<class T>
std::vector<Tree<T>*>& Tree<T>::getRecentVecTree(){
    if(changeY){
        return recentDataTreeY;
    }else{
        return recentDataTreeX;
    }
}

template<class T>
void Tree<T>::setRecentVec(std::vector<Tree<T>*> r){
    if(changeY){
        recentDataTreeY = r;
    }else{
        recentDataTreeX = r;
    }
}

template<class T>
int Tree<T>::getRecentTreeSize(){
    if(changeY){
        return recentDataTreeY.size();
    }else{
        return recentDataTreeX.size();
    }
}

template<class T>
bool Tree<T>::notInRecentTree(Tree<T>* t){
    std::vector<Tree<T>*>& recentDataTree = recentDataTreeX;
    if(changeY){
        recentDataTree = recentDataTreeY;
    }
    for(int i = 0; i<(int)recentDataTree.size(); i++){
        if(recentDataTree[i] == t){
            return false;
        }
    }
    return true;
}

template<class T>
void Tree<T>::insertNode(T* firstHalf, T* secondHalf, std::vector<T*> datas, int nbNode, int connNode){
    std::cerr << "ENTREE INSERT NODE" << std::endl;
    std::vector<Tree<T>*> newChildren;
    std::vector<Tree<T>*> oldChildren = children;
    this->value = firstHalf;
    for(int i = 0; i<nbNode; i++){
        newChildren.push_back(new Tree<T>(datas[i]));
        ((newChildren[i])->parents).push_back(this);
    }
    children = newChildren;
    (children[connNode])->addChild(new Tree<T>(secondHalf));
    ((*(children[connNode]))[0])->children = oldChildren;
    ((*(children[connNode]))[0])->addParent(children[connNode]);
    for(int i = 0; i<(int)oldChildren.size(); i++){
        (oldChildren[i])->clearParents();
        (oldChildren[i])->addParent((*(children[connNode]))[0]);
    }
    std::cerr << "SORTIE INSERT NODE" << std::endl;
}

template<class T>
Tree<T>* Tree<T>::operator[](int ind){
    if(ind < 0 || ind >= (int)children.size()){
        std::cerr << "Indice d'enfant qui ne convient pas" << std::endl;
    }
    return children[ind];
}

template<class S>
std::ostream& operator<<(std::ostream& out, const Tree<S>& t){
    out << *(t.value) << "-> ";
    if(t.isLeaf){
        out << "X" << std::endl;
    }else{
        out << std::endl << "____" << std::endl;
    }
    for(int i = 0; i<t.childrenSize; i++){
       out << *(t.children[i]);
    }
    if(!t.isLeaf){
        out << "____" << std::endl;
    }
    return out;
}


#endif // TREE_H
