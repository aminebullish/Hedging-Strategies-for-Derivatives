#pragma once

#include "Option.hpp"

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class Asian : public Option
{
public:
    double div_N;
    PnlVect *col;
    Asian(double T, int nbTimeSteps, int size, double strike, PnlVect *lambdas);
    ~Asian();
    double payoff(const PnlMat *path);
};
