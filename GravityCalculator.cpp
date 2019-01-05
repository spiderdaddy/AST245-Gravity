//
// Created by lf on 03/01/19.
//
#include <chrono>

#include "GravityCalculator.h"

GravityCalculator::GravityCalculator( Structure *structure ) {
    this->structure = structure;
}

void GravityCalculator::setStructure(Structure *structure) {
    this->structure = structure;
}

void GravityCalculator::calculate() {
    startTimer();
    _calculate();
    stopTimer();
}

void GravityCalculator::startTimer() {
    startTime = std::chrono::system_clock::now();
}

void GravityCalculator::stopTimer() {
    endTime = std::chrono::system_clock::now();

}

long GravityCalculator::getTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}
