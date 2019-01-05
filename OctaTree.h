//
// Created by lf on 05/01/19.
//

#ifndef GRAVITY_OCTATREE_H
#define GRAVITY_OCTATREE_H

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

struct OctaTreeNode {

public:
    OctaTreeNode( OctaTreeNode *parent,
                  unsigned level,
                  unsigned X, unsigned Y, unsigned Z
                  double h);

    OctaTreeNode * parent;
    OctaTreeNode * leaf[8];
    unsigned level;
    // grid coordinates
    unsigned X, Y, Z;
    double V;
    double mass;
    double xmin, xmax, ymin, ymax, zmin, zmax;
    double x, y, z;
};

class OctaTree {

public:
    OctaTree(unsigned max_level, double h);
    void printNodes();
    OctaTreeNode * getHead();
    std::vector<OctaTreeNode*> getLevelVector(unsigned level);
    unsigned getMaxLevel();

private:
    OctaTreeNode * createNode( OctaTreeNode * parent,
                               unsigned level,
                               unsigned X, unsigned Y, unsigned Z
                               double h );
    void printOneNode(OctaTreeNode*);
    std::vector<std::vector<OctaTreeNode*>> levelVector;
    void printNodesPerLevel();

    unsigned max_level = 0;
    OctaTreeNode *head;
    double h;

};


#endif //GRAVITY_OCTATREE_H
