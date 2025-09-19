#include "AsianOption.hpp"

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"


Asian::Asian(double T, int nbTimeSteps, int size, double strike, PnlVect *lambdas)
:Option(T, nbTimeSteps, size, strike, lambdas)
{
    div_N = 1 / (double) (nbTimeSteps + 1);
    col = pnl_vect_create(nbTimeSteps + 1);
}

Asian::~Asian()
{
    pnl_vect_free(&col);
}

double Asian::payoff(const PnlMat *path)
{
    double result;
    double sum = 0.;
 
    for(int j = 0; j < size_; j++)
    {
        pnl_mat_get_col(col, path, j);
        sum += pnl_vect_sum(col) * GET(lambdas_, j);
    }
    
    result = sum * div_N  - strike_;
    return (result > 0.) ? result : 0;
}


