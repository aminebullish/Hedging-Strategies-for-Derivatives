#include "../MonteCarlo.hpp"
#include "../Option.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include "../BlackScholesModel.hpp"
#include "../BasketOption.hpp"
#include <iostream>
#include "pnl/pnl_finance.h"

int main(){
    int nbTimesteps = 1;
    int size = 1;
    double vol = 0.2;
    double initspot = 100;
    double r = 0.04879;
    double T = 1;
    double rho = 0;
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    PnlVect *sigma = pnl_vect_create_from_scalar(size,vol);
    PnlVect *spot = pnl_vect_create_from_scalar(size,initspot);
    BlackScholesModel *pbs = new BlackScholesModel(size, r, rho, sigma, spot);
    double h = 0.1;
    int nbSamples = 5E4;
    double lambda = 1;
    PnlVect *lambdas = pnl_vect_create_from_scalar(size, lambda);
    double strike = 100;
    Basket *popt = new Basket(T, nbTimesteps, size, strike, lambdas);
    MonteCarlo ms = MonteCarlo(pbs, popt, rng, h, nbSamples);
    PnlVect *delta = pnl_vect_create(popt->size_);
    PnlVect *std_dev = pnl_vect_create(popt->size_);
    ms.delta(delta, std_dev);

    double price = 0;
    double delta_pnl = 0;
    pnl_cf_call_bs(100, 100, 1, 0.04879, 0, 0.2, &price, &delta_pnl);
    std::cout << "delta value using:\n      closed formulat: " << pnl_vect_get(delta, 0) <<"\n      Monte Carlo: " << delta_pnl  << std::endl;
    std::cout << "Error: "<< 100*(delta_pnl - pnl_vect_get(delta, 0))/delta_pnl << "%" << std::endl;


    
}