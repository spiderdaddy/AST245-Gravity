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

#include "Structure.h"

struct OctaTreeNode {

public:
    OctaTreeNode( OctaTreeNode *parent,
                  Structure * s,
                  std::vector<unsigned> * element_candidates,
                  unsigned level,
                  unsigned X, unsigned Y, unsigned Z,
                  double h);
    ~OctaTreeNode();

    OctaTreeNode * parent;
    OctaTreeNode * leaf[8];
    std::vector<unsigned> elements;
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
    OctaTree(Structure * structure);
    void printNodes();
    OctaTreeNode * getHead();
    // std::vector<OctaTreeNode*> getLevelVector(unsigned level);
    unsigned getMaxLevel();
    // OctaTreeNode *getNode(double x, double y, double z);

private:
    OctaTreeNode * createNode( OctaTreeNode * parent,
                               std::vector<unsigned> * element_candidates,
                               unsigned level,
                               unsigned X, unsigned Y, unsigned Z);
    void printNodeRecursive(OctaTreeNode *);
    void printNodeRecursive(OctaTreeNode *, double max_angle);
    //std::vector<std::vector<OctaTreeNode*>> levelVector;
    void printNodesPerLevel();
    unsigned getWidth(unsigned level);
    unsigned getPos(unsigned level, unsigned X, unsigned Y, unsigned Z);

    unsigned max_level = 0;
    OctaTreeNode *head;
    Structure * s;

};


#endif //GRAVITY_OCTATREE_H
