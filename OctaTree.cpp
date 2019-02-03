//
// Created by lf on 05/01/19.
//

#include "OctaTree.h"

OctaTreeNode::OctaTreeNode(
        OctaTreeNode *parent,
        Structure * s,
        std::vector<unsigned> * element_candidates,
        unsigned level,
        unsigned X, unsigned Y, unsigned Z,
        double h
) {
    this->parent = parent;
    this->level = level;
    this->X = X;
    this->Y = Y;
    this->Z = Z;
    //this->elements.reserve(element_candidates->size()/8);

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
    x = 0;
    y = 0;
    z = 0;

    std::for_each(element_candidates->begin(), element_candidates->end(),
                  [s, this](unsigned i){
                      auto pos = s->getPositions()[i].pos;
                      if (
                             (pos.x >= xmin)
                          && (pos.x < xmax)
                          && (pos.y >= ymin)
                          && (pos.y < ymax)
                          && (pos.z >= zmin)
                          && (pos.z <= zmax)
                              ) {
                          elements.push_back(i);
                          mass += s->getMasses()[i].m;
                          x += pos.x;
                          y += pos.y;
                          z += pos.z;
                      }
                  });
    x /= (double)elements.size();
    y /= (double)elements.size();
    z /= (double)elements.size();

}

OctaTreeNode::~OctaTreeNode() {
    for (unsigned i = 0; i < 8; i++) {
        if (this->leaf[i] != nullptr) {
            delete this->leaf[i];
            this->leaf[i] = nullptr;
        }

    }
}

std::ostream &operator<<(std::ostream &os, const OctaTreeNode *node) {
    os << (void *) node << ": "
       << std::setw(2) << node->level;

    auto node_counter = (OctaTreeNode*)node;
    while( node_counter->parent != nullptr ) {
        os << " |  ";
        node_counter = node_counter->parent;
    }

    os << " ("
       << node->X << ", " << node->Y << ", " << node->Z << "), ("
       << node->elements.size() << "), ("
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

    os << "["<<((node->xmax-node->xmin)/2.0) / std::sqrt(node->x*node->x+node->y*node->y+node->z*node->z)<<"]";

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

    this->s = s;

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
    std::vector<unsigned> all_elements(s->getPositions().size());
    std::iota(all_elements.begin(), all_elements.end(), 0);

    head = createNode((OctaTreeNode *) nullptr, &all_elements, 0, 0, 0, 0);
    auto endTime = std::chrono::system_clock::now();
    std::cout << "Tree creation time:" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "\n";

}

OctaTreeNode *OctaTree::createNode(OctaTreeNode * parent,
                                   std::vector<unsigned> * element_candidates,
                                   unsigned level,
                                   unsigned X, unsigned Y, unsigned Z ) {

    auto node = new OctaTreeNode( parent, s, element_candidates, level, X, Y, Z, s->getH() );
    // levelVector[level][getPos(level, X, Y, Z)] = node;

    if (node->elements.size() > 1) {

        // If there is more than one element then break down further

        for (unsigned i = 0; i < 8; i++ ) {

            // The formula for the nodes is to double the current position
            // and add the binary bit of the index
            node->leaf[i] = createNode(node,
                                       &node->elements,
                                       level + 1,
                                       (X*2) + (i&1),
                                       (Y*2) + ((i>>1)&1),
                                       (Z*2) + ((i>>2)&1)
            );
        }
    } else if (node->elements.size() == 1) {

        // If there is one element then stop here
        return node;
    } else {

        // If this node is empty then stop here
        delete node;
        return nullptr;
    }

    return node;
}

void OctaTree::printNodeRecursive(OctaTreeNode *node) {
    std::cout << node << "\n";
    if (node != nullptr) {
        for (int i = 0; i < 8; i++) {
            if (node->leaf[i] != nullptr) {
                printNodeRecursive(node->leaf[i]);
            }
        }
    }
}

void OctaTree::printNodeRecursive(OctaTreeNode *node, double max_angle) {
    std::cout << node << "\n";
    if ( (node != nullptr)
         &&  ((((node->xmax-node->xmin)/2.0)
               / std::sqrt(node->x*node->x+node->y*node->y+node->z*node->z)) > max_angle )
            ) {
        for (int i = 0; i < 8; i++) {
            if (node->leaf[i] != nullptr) {
                printNodeRecursive(node->leaf[i], max_angle);
            }
        }
    }
}


/*
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
*/

void OctaTree::printNodes() {

    auto startTime = std::chrono::system_clock::now();

    std::streambuf *psbuf, *backup;
    std::ofstream filestr;
    std::string filename("OctaTree.txt");
    filestr.open(filename);

    backup = std::cout.rdbuf();     // back up cout's streambuf

    psbuf = filestr.rdbuf();        // get file's streambuf
    std::cout.rdbuf(psbuf);         // assign streambuf to cout

    printNodeRecursive(head);
    // printNodesPerLevel();

    std::cout.rdbuf(backup);        // restore cout's original streambuf

    filestr.close();
    auto endTime = std::chrono::system_clock::now();

    std::cout << "INFO: OctaTree written to " << filename << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "\n";
    ;


}

OctaTreeNode *OctaTree::getHead() {
    return head;
}

/*
std::vector<OctaTreeNode *> OctaTree::getLevelVector(unsigned level) {
    return levelVector[level];

}
*/

unsigned OctaTree::getMaxLevel() {
    return max_level;
}

/*
OctaTreeNode *OctaTree::getNode(double x, double y, double z) {
    auto cell_width = 2.0 * this->h / getWidth(this->max_level);
    auto X = (unsigned)((x+h) / cell_width);
    auto Y = (unsigned)((y+h) / cell_width);
    auto Z = (unsigned)((z+h) / cell_width);
    return levelVector[max_level][getPos(max_level, X, Y, Z)];
}
*/


