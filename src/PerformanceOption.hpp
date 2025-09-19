#pragma once

#include "Option.hpp"

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class Performance : public Option
{
public:
    PnlVect *temp_vect;
    Performance(double T, int nbTimeSteps, int size, double strike, PnlVect *lambdas);
    ~Performance();
    double payoff(const PnlMat *path);
};
