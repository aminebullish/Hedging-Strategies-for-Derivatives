#pragma once

#include "Option.hpp"

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class Basket : public Option
{
public:
    PnlVect *last_spot;
    Basket(double T, int nbTimeSteps, int size, double strike, PnlVect *lambdas);
    ~Basket();
    double payoff(const PnlMat *path);
};
