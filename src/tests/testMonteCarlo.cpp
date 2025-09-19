#include "../MonteCarlo.hpp"
#include "../Option.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include "../BlackScholesModel.hpp"
#include "../AsianOption.hpp"
#include <iostream>

int main(){
    int nbTimesteps = 10;
    int size = 2;
    double vol = 0.2;
    double initspot = 102;
    double r = 0.02;
    double T = 1.5;
    double rho = 0;
    PnlVect *sigma = pnl_vect_create_from_scalar(size,vol);
    PnlVect *spot = pnl_vect_create_from_scalar(size,initspot);
    // BlackScholesModel bs = BlackScholesModel(size, r, rho, sigma, spot);
    BlackScholesModel *pbs = new BlackScholesModel(size, r, rho, sigma, spot);
    PnlMat *path = pnl_mat_create_from_zero(nbTimesteps + 1, size);
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    pbs->asset(path, T, nbTimesteps, rng);    
    double h = 0.1;
    int nbSamples = 5E4;
    double lambda = 0.5;
    PnlVect *lambdas = pnl_vect_create_from_scalar(size, lambda);
    double strike = 100;
    Asian *popt = new Asian(T, nbTimesteps, size, strike, lambdas);
    // Asian *popt = &opt;
    MonteCarlo ms = MonteCarlo(pbs, popt, rng, h, nbSamples);
    double prix = 0;
    double std_dev = 0;
    ms.price(prix, std_dev);

    std::cout << "Prix_t0 = " << prix << std::endl;
    std::cout << "Ecart type = " << std_dev << std::endl;



    //////////////////////test delta à tout t///////////////////
    double t = 0.5;
    PnlMat *past = pnl_mat_create_from_scalar(1, size, 100);
    PnlVect *delta = pnl_vect_create_from_zero(size);
    PnlVect *std_dev_vect = pnl_vect_create_from_zero(size);
    ms.delta(past, t, delta, std_dev_vect);
    for(int i=0; i<size; i++)
    {
        printf("%f         ",GET(delta, i));    
    }

    printf("\n");
    
    for(int i=0; i<size; i++)
    {
        printf("%f         ",GET(std_dev_vect, i));
    }
    ////////////////test Profit and Loss ////////////

    // std::cout << "\nP&L = " << ms.hedgingPAndL(path, path->m - 1) << std::endl;    
    delete pbs;
    delete popt;
    pnl_vect_free(&lambdas);
    pnl_vect_free(&sigma);
    pnl_vect_free(&spot);
    pnl_mat_free(&path);
    pnl_rng_free(&rng);
    pnl_vect_free(&delta);
    pnl_vect_free(&std_dev_vect);
    pnl_mat_free(&past);

    std::cout << "\nTest Passed\n" << std::endl;
    return 0;
}