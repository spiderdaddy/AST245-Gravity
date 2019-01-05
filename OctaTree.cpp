//
// Created by lf on 05/01/19.
//

#include "OctaTree.h"

OctaTreeNode::OctaTreeNode(
        OctaTreeNode *parent,
        unsigned level,
        unsigned X, unsigned Y, unsigned Z,
        double h
) {
    this->parent = parent;
    this->level = level;
    this->X = X;
    this->Y = Y;
    this->Z = Z;

    std::fill(leaf, leaf + 8, nullptr);

    if (level != 0) {
        xmin = -h + (double) X / double(level) * h;
        xmax = -h + (double) (X + 1) / double(level) * h;
        ymin = -h + (double) Y / double(level) * h;
        ymax = -h + (double) (Y + 1) / double(level) * h;
        zmin = -h + (double) Y / double(level) * h;
        zmax = -h + (double) (Z + 1) / double(level) * h;
    } else {
        // XXX: Bah, come up with a better formula to account for div by 0
        xmin = -h ;
        xmax = h;
        ymin = -h;
        ymax = h;
        zmin = -h;
        zmax = h;
    }
    V = (xmax-xmin)*(xmax-xmin)*(xmax-xmin);
    mass = 0;
    x = (xmax+xmin)/2.0;
    y = (ymax+ymin)/2.0;
    z = (zmax+zmin)/2.0;
}

std::ostream &operator<<(std::ostream &os, const OctaTreeNode *node) {
    os << (void *) node << ": "
       << node->level;

    auto node_counter = (OctaTreeNode*)node;
    while( node_counter->parent != nullptr ) {
        os << "|   ";
        node_counter = node_counter->parent;
    }

    os << (void *) node->parent << ", (";
    for (int i = 0; i < 7; i++) {
        os << (void *) node->leaf[i] << ", ";
    }
    os << (void *) node->leaf[7] << "), ("
       << node->X << ", " << node->X << ", " << node->Z << "), ("
       << node->x << ", " << node->y << ", " << node->z << "), ("
       << node->xmin << ", " << node->xmax << "), ("
       << node->ymin << ", " << node->ymax << "), ("
       << node->zmin << ", " << node->zmax << "), ("
       << node->V << ", " << node->mass << ")";

    return os;
}

OctaTree::OctaTree(unsigned max_level, double h) {
    this->max_level = max_level;
    this->h = h;

    // Create the list of vectors which will contain the nodes for each level
    levelVector = std::vector<std::vector<OctaTreeNode *>>(max_level+1);

    // Create the vectors for each level
    for (int i = 0; i < (max_level + 1); i++) {
        levelVector[i] = std::vector<OctaTreeNode *>();
        levelVector[i].reserve((i+1)*(i+1)*(i+1));
    }

    // Recurseively create the node structure
    head = createNode((OctaTreeNode *) nullptr, 0, 0, 0, 0, h);

}

OctaTreeNode *OctaTree::createNode(OctaTreeNode * parent,
                                   unsigned level,
                                   unsigned X, unsigned Y, unsigned Z
                                   double h) {

    auto node = new OctaTreeNode( parent, level, X, Y, Z, h);
    levelVector[level].push_back(node);

    if (level < max_level) {
        for (int i = 0; i < 8; i++ ) {

            // The formula for the nodes is to double the current position
            // and add the binary bit of the index
            node->leaf[i] = createNode(node,
                                       level + 1,
                                       (X*2) + i&1, (Y*2) + i&2, (Z*2) + Y&4,
                                       h
            );
        }
    }

    return node;
}

void OctaTree::printOneNode(OctaTreeNode *node) {
    std::cout << node << "\n";
    if (node != nullptr) {
        for (int i = 0; i < 4; i++) {
            if (node->leaf[i] != nullptr) {
                printOneNode(node->leaf[i]);
            }
        }
    }
}

void OctaTree::printNodesPerLevel() {
    int level = 0;
    for (std::vector<OctaTreeNode *> nodes : levelVector) {
        std::cout << "Level:" << (max_level - level) << "\n";
        int linebreak = 0;
        for (OctaTreeNode *node : nodes) {
            std::cout << (void *) node << ",";
            if (linebreak++ > 14) {
                std::cout << "\n";
                linebreak = 0;
            }
        }
        std::cout << "\n";
        level++;
    }
}

void OctaTree::printNodes() {

    std::streambuf *psbuf, *backup;
    std::ofstream filestr;
    std::string filename("OctaTree.txt");
    filestr.open(filename);

    backup = std::cout.rdbuf();     // back up cout's streambuf

    psbuf = filestr.rdbuf();        // get file's streambuf
    std::cout.rdbuf(psbuf);         // assign streambuf to cout

    printOneNode(head);
    printNodesPerLevel();

    std::cout.rdbuf(backup);        // restore cout's original streambuf

    filestr.close();

    std::cout << "INFO: OctaTree written to " << filename;


}

OctaTreeNode *OctaTree::getHead() {
    return head;
}

std::vector<OctaTreeNode *> OctaTree::getLevelVector(unsigned level) {
    return levelVector[level];

}

int OctaTree::getMaxLevel() { return max_level; }