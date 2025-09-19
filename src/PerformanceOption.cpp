#include "PerformanceOption.hpp"

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"


Performance::Performance(double T, int nbTimeSteps, int size, double strike, PnlVect *lambdas)
:Option(T, nbTimeSteps, size, strike, lambdas)
{
    temp_vect = pnl_vect_create(size);
}

Performance::~Performance()
{
    pnl_vect_free(&temp_vect);
}

double Performance::payoff(const PnlMat *path)
{
    double perf, sum1, sum2;
    double result = 0.;

    pnl_mat_get_row(temp_vect, path, 0);
    sum2 = pnl_vect_scalar_prod(lambdas_, temp_vect);

    for(int i = 1; i < nbTimeSteps_ + 1; i++)
    {
        pnl_mat_get_row(temp_vect, path, i);
        sum1 = pnl_vect_scalar_prod(lambdas_, temp_vect);

        perf = sum1 / sum2;
        if(perf > 1.)
        {
            result += perf - 1.;
        }
        
        sum2 = sum1;       
    }
    return 1. + result;
}
