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
        double segments = pow(2, level);
        double Xpos = X * 2;
        double Ypos = Y * 2;
        double Zpos = Z * 2;
        xmin = -h + Xpos / segments * h;
        xmax = -h + (Xpos + 2) / segments * h;
        ymin = -h + Ypos / segments * h;
        ymax = -h + (Ypos + 2) / segments * h;
        zmin = -h + Zpos / segments * h;
        zmax = -h + (Zpos + 2) / segments * h;
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
        os << " |  ";
        node_counter = node_counter->parent;
    }

    os << " ("
       << node->X << ", " << node->Y << ", " << node->Z << "), ("
       << node->x << ", " << node->y << ", " << node->z << "), ("
       << node->xmin << ", " << node->xmax << "), ("
       << node->ymin << ", " << node->ymax << "), ("
       << node->zmin << ", " << node->zmax << "), ("
       << node->V << ", " << node->mass << "), ";

    os << (void *) node->parent << ", (";
    for (int i = 0; i < 7; i++) {
        os << (void *) node->leaf[i] << ", ";
    }
    os << (void *) node->leaf[7] << ")";

    return os;
}

unsigned OctaTree::getWidth(unsigned level) {
    return (unsigned)pow(2, level);
}

unsigned OctaTree::getPos(unsigned level, unsigned X, unsigned Y, unsigned Z) {
    auto width = getWidth(level);
    return Z*(width*width) + (Y*width) + X;
}

OctaTree::OctaTree(Structure * s) {

    auto startTime = std::chrono::system_clock::now();

    // Create the list of vectors which will contain the nodes for each level
    // XXX levelVector = std::vector<std::vector<OctaTreeNode *>>(max_level+1);

    // Create the vectors for each level
    /* XXX
    for (unsigned i = 0; i < (max_level + 1); i++) {
        levelVector[i] = std::vector<OctaTreeNode *>();
        auto width = getWidth(i);
        levelVector[i].reserve(width*width*width);
    }
    */

    // Recurseively create the node structure
    head = createNode((OctaTreeNode *) nullptr, 0, 0, 0, 0, h);
    auto endTime = std::chrono::system_clock::now();
    std::cout << "Tree creation time:" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "\n";

}

OctaTreeNode *OctaTree::createNode(OctaTreeNode * parent,
                                   unsigned level,
                                   unsigned X, unsigned Y, unsigned Z,
                                   double h) {

    auto node = new OctaTreeNode( parent, level, X, Y, Z, h );
    levelVector[level][getPos(level, X, Y, Z)] = node;

    if (level < max_level) {
        for (unsigned i = 0; i < 8; i++ ) {

            // The formula for the nodes is to double the current position
            // and add the binary bit of the index
            node->leaf[i] = createNode(node,
                                       level + 1,
                                       (X*2) + (i&1),
                                       (Y*2) + ((i>>1)&1),
                                       (Z*2) + ((i>>2)&1),
                                       h
            );
        }
    }

    return node;
}

void OctaTree::printOneNode(OctaTreeNode *node) {
    std::cout << node << "\n";
    if (node != nullptr) {
        for (int i = 0; i < 8; i++) {
            if (node->leaf[i] != nullptr) {
                printOneNode(node->leaf[i]);
            }
        }
    }
}

void OctaTree::printNodesPerLevel() {
    int level = 0;
    for (std::vector<OctaTreeNode *> nodes : levelVector) {
        std::cout << "Level:" << level << "\n";
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

    auto startTime = std::chrono::system_clock::now();

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
    auto endTime = std::chrono::system_clock::now();

    std::cout << "INFO: OctaTree written to " << filename << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "\n";
    ;


}

OctaTreeNode *OctaTree::getHead() {
    return head;
}

std::vector<OctaTreeNode *> OctaTree::getLevelVector(unsigned level) {
    return levelVector[level];

}

unsigned OctaTree::getMaxLevel() {
    return max_level;
}

OctaTreeNode *OctaTree::getNode(double x, double y, double z) {
    auto cell_width = 2.0 * this->h / getWidth(this->max_level);
    auto X = (unsigned)((x+h) / cell_width);
    auto Y = (unsigned)((y+h) / cell_width);
    auto Z = (unsigned)((z+h) / cell_width);
    return levelVector[max_level][getPos(max_level, X, Y, Z)];
}



