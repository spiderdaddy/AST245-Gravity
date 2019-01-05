//
// Created by lf on 03/01/19.
//

#ifndef GRAVITY_GRAVITYCALCULATOR_H
#define GRAVITY_GRAVITYCALCULATOR_H

#include <chrono>
#include "Structure.h"


class GravityCalculator {

public:

    GravityCalculator()= default;

    GravityCalculator( Structure * structure );

    void setStructure( Structure * structure );

    void calculate();
    long getTime();

protected:

    Structure * structure = nullptr;
    void startTimer();
    void stopTimer();
    virtual void _calculate()=0;

    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;

};


#endif //GRAVITY_GRAVITYCALCULATOR_H
