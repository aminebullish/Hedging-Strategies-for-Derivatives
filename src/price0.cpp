#include <iostream>
#include <string>
#include "3rdparty/jlparser/include/jlparser/parser.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include "Option.hpp"
#include "AsianOption.hpp"
#include "BasketOption.hpp"
#include "PerformanceOption.hpp"
#include "BlackScholesModel.hpp"
#include "MonteCarlo.hpp"
#include "PricingResults.hpp"

using namespace std;

int main(int argc, char **argv)
{
    double T, r, strike, rho, h;
    PnlVect *spot, *sigma, *lambdas;
    string type;
    int size, N;
    size_t M;

    char *infile = argv[1];
    Param *P = new Parser(infile);

    P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    if (P->extract("fd step", h, true) == false)
    {
        h = 0.1;
    }
    P->extract("sample number", M);
    P->extract("correlation", rho);
    P->extract("payoff coefficients", lambdas, size);
    P->extract("timestep number", N);
    
    Option *opt;
    if (type == "asian") {
        P->extract("strike", strike);
        opt = new Asian(T, N, size, strike, lambdas);
    } else if (type == "basket") {
        P->extract("strike", strike);
        opt = new Basket(T, N, size, strike, lambdas);
    } else if (type == "performance") {
        opt = new Performance(T, N, size, 0., lambdas);
    } else {
        std::cout << "Unknown option type" << std::endl;
        exit(1); 
    }

    BlackScholesModel *bs = new BlackScholesModel(size, r, rho, sigma, spot);
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    MonteCarlo mc(bs, opt, rng, h, (int) M);

    double prix, prix_std_dev;
    PnlVect *delta = pnl_vect_create_from_zero(size);
    PnlVect *delta_std_dev = pnl_vect_create_from_zero(size);
    mc.price(prix, prix_std_dev);
    mc.delta(delta, delta_std_dev);

    PricingResults res(prix, prix_std_dev, delta, delta_std_dev);
    std::cout << res << std::endl;

    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&lambdas);
    pnl_vect_free(&delta);
    pnl_vect_free(&delta_std_dev);
    pnl_rng_free(&rng);
    delete P;
    delete bs;
    delete opt;

    exit(0);
}