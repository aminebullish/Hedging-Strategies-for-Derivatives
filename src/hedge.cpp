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
#include "HedgingResults.hpp"

using namespace std;

int main(int argc, char **argv)
{
    double T, r,strike, rho, h;
    PnlVect *spot, *sigma, *lambdas, *trend;
    string type;
    int size, N;
    size_t M;
    int H;

    char *market_file = argv[1];
    char *infile = argv[2];
    Param *P = new Parser(infile);

    P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("trend", trend, size);
    if (P->extract("fd step", h, true) == false)
    {
        h = 0.1;
    }
    P->extract("sample number", M);
    P->extract("correlation", rho);
    P->extract("payoff coefficients", lambdas, size);
    P->extract("timestep number", N);
    P->extract("hedging dates number", H);
    P->extract("interest rate", r);

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

    BlackScholesModel *bs = new BlackScholesModel(size, r, rho, sigma, spot, trend);
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    MonteCarlo mc(bs, opt, rng, h, (int) M);

    // market data from file
    PnlMat *path = pnl_mat_create_from_file(market_file);

    // market data simulated
    /*
    PnlMat *path = pnl_mat_create_from_zero(H+1,  size);
    bs->simul_market(path, T, H, rng);
    */

    double prix, prix_std_dev, erreur_couverture; 
    mc.hedgingPAndL(path, H, prix, prix_std_dev, erreur_couverture);

    HedgingResults res(prix, prix_std_dev, erreur_couverture);
    std::cout << res << std::endl;

    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&lambdas);
    pnl_rng_free(&rng);
    pnl_vect_free(&trend);
    pnl_mat_free(&path);
    delete P;
    delete bs;
    delete opt;

    exit(0);
}