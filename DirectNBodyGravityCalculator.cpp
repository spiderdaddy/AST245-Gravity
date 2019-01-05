//
// Created by lf on 03/01/19.
//

#include "DirectNBodyGravityCalculator.h"

void DirectNBodyGravityCalculator::_calculate() {

    std::vector<Structure::Acceleration> &acc = structure->getAccelerations();
    std::vector<Structure::Position> &pos = structure->getPositions();



    // Calculate acceleration for all points
    for (std::size_t i = 0; i < acc.size(); i++) {

        Structure::XYZ &a = acc.at(i).a;

        a = { 0.0f, 0.0f, 0.0f };

        for (std::size_t j = 0; j < acc.size(); j++) {

            if (i==j) {
                continue;
            }

            // Assumption: Not optimising for computational efficiency, i.e. pre defining variables etc.
            // Assumption: All masses are 1
            // Assumption: G = 1

            float dx = pos[j].pos.x - pos[i].pos.x;
            float dy = pos[j].pos.y - pos[i].pos.y;
            float dz = pos[j].pos.z - pos[i].pos.z;
            float r = std::sqrt(dx*dx+dy*dy+dz*dz);

            float g = softening(r);
            a.x += g * dx / r;
            a.y += g * dy / r;
            a.z += g * dz / r;

        }
    }
}

void DirectNBodyGravityCalculator::setSofteningEpsilon(float epsilon) {
    DirectNBodyGravityCalculator::epsilon = epsilon;
}

float DirectNBodyGravityCalculator::softening(float r) {

    /*
     * Calculates F as
     *
     *       - r
     *  -----------------
     *  (r^2 + e^2)^(3/2)
     *
     */
    float x = r*r + epsilon*epsilon;
    return -r / (x * std::sqrt(x));
}
