//
// Created by lf on 03/01/19.
//

#ifndef GRAVITY_DIRECTNBODYGRAVITYCALCULATOR_H
#define GRAVITY_DIRECTNBODYGRAVITYCALCULATOR_H

#include <cstdlib>
#include <iostream>
#include <cmath>

#include "GravityCalculator.h"


class DirectNBodyGravityCalculator : public GravityCalculator {

public:

    explicit DirectNBodyGravityCalculator ( Structure * structure ) : GravityCalculator ( structure ) {}

    void setSofteningEpsilon(float epsilon);

protected:
    void _calculate() override ;

private:
    float epsilon = 0;
    float softening(float r);

};


#endif //GRAVITY_DIRECTNBODYGRAVITYCALCULATOR_H
