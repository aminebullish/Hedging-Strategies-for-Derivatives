#include "BasketOption.hpp"

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"


Basket::Basket(double T, int nbTimeSteps, int size, double strike, PnlVect *lambdas)
:Option(T, nbTimeSteps, size, strike, lambdas)
{
    last_spot = pnl_vect_create(size);
}

Basket::~Basket()
{
    pnl_vect_free(&last_spot);
}

double Basket::payoff(const PnlMat *path)
{
    double result;

    pnl_mat_get_row(last_spot, path, nbTimeSteps_);
    result = pnl_vect_scalar_prod(lambdas_, last_spot) - strike_;
    
    return (result > 0.) ? result : 0;
}


