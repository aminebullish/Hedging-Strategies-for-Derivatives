#include "MonteCarlo.hpp"
#include "BlackScholesModel.hpp"

#include "pnl/pnl_random.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include <iostream>

MonteCarlo::MonteCarlo(BlackScholesModel *mod, Option *opt, PnlRng *rng, double fdStep, int nbSamples)
    : mod_(mod)
    , opt_(opt)
    , rng_(rng)
    , fdStep_(fdStep)
    , nbSamples_(nbSamples)
{
    div_M = 1 / (double) nbSamples_;
    timestep = opt_->T_/ (double) opt_->nbTimeSteps_;
    path = pnl_mat_create(opt_->nbTimeSteps_ + 1, opt_->size_);
    shift_path_plus = pnl_mat_create(opt_->nbTimeSteps_ + 1, opt_->size_);
    shift_path_moins = pnl_mat_create(opt_->nbTimeSteps_ + 1, opt_->size_);
}

MonteCarlo::~MonteCarlo()
{
    pnl_mat_free(&path);
    pnl_mat_free(&shift_path_plus);
    pnl_mat_free(&shift_path_moins);
}

void MonteCarlo::price(double &prix, double &std_dev)
{
    prix = 0;
    std_dev = 0;
    double tmp;
    double dis = exp(-(mod_->r_)*(opt_->T_));

    for (int i = 0; i < nbSamples_; i++)
    {
        mod_->asset(path, opt_->T_, opt_->nbTimeSteps_, rng_);
        tmp = opt_->payoff(path);
        prix += tmp;
        std_dev += tmp * tmp;
    }

    prix *= div_M;
    std_dev = std_dev * div_M - prix * prix;
    
    prix *= dis;
    std_dev = sqrt(std_dev * div_M) * dis;
}

void MonteCarlo::price(const PnlMat *past, double t, double &prix, double &std_dev)
{
    prix = 0;
    std_dev = 0;
    double tmp;
    double dis = exp(-(mod_->r_)*(opt_->T_ - t));

    for (int i = 0; i < nbSamples_; i++)
    {
        mod_->asset(path, t, opt_->T_, opt_->nbTimeSteps_, rng_, past);
        tmp = opt_->payoff(path);
        prix += tmp;
        std_dev += tmp * tmp;
    }

    prix *= div_M;
    std_dev = std_dev * div_M - prix * prix;
    
    prix *= dis;
    std_dev = sqrt(std_dev * div_M) * dis;
}

void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *std_dev)
{
    double tmp, fact;
    double dis = exp(-(mod_->r_)*(opt_->T_ - t));
    pnl_vect_mult_scalar(delta, 0);
    pnl_vect_mult_scalar(std_dev, 0);

    for (int i = 0; i < nbSamples_; i++)
    {
        mod_->asset(path, t, opt_->T_, opt_->nbTimeSteps_, rng_, past);

        for (int d = 0; d < opt_->size_; d++) 
        {
            mod_->shiftAsset(shift_path_plus, path, d, fdStep_, t, timestep);
            mod_->shiftAsset(shift_path_moins, path, d, - fdStep_, t, timestep);
            tmp = opt_->payoff(shift_path_plus) - opt_->payoff(shift_path_moins);           
            LET(delta, d) += tmp;
            LET(std_dev, d) += tmp * tmp;
        }    
    }

    for (int d = 0; d < opt_->size_; d++)
    {
        fact = dis * div_M / (MGET(past, past->m-1, d) * fdStep_ * 2.);
        tmp = GET(delta, d);
        LET(delta, d) *= fact;
        LET(std_dev, d) = sqrt(GET(std_dev, d) - tmp * tmp * div_M) * fact;
    }
}

void MonteCarlo::delta(PnlVect *delta, PnlVect *std_dev)
{
    double tmp, fact;
    double dis = exp(-(mod_->r_)*(opt_->T_));
    pnl_vect_mult_scalar(delta, 0);
    pnl_vect_mult_scalar(std_dev, 0);
    
    for (int i = 0; i < nbSamples_; i++)
    {
        mod_->asset(path, opt_->T_, opt_->nbTimeSteps_, rng_);

        for (int d = 0; d < opt_->size_; d++) 
        {
            mod_->shiftAsset(shift_path_plus, path, d, fdStep_, 0, timestep);
            mod_->shiftAsset(shift_path_moins, path, d, - fdStep_, 0, timestep);
            tmp = opt_->payoff(shift_path_plus) - opt_->payoff(shift_path_moins);
            LET(delta, d) += tmp;
            LET(std_dev, d) += tmp * tmp;
        }    
    }

    for (int d = 0; d < opt_->size_; d++)
    {
        fact = dis * div_M / (GET(mod_->spot_, d) * fdStep_ * 2.);
        tmp = GET(delta, d);
        LET(delta, d) *= fact;
        LET(std_dev, d) = sqrt(GET(std_dev, d) - tmp * tmp * div_M) * fact;
    }
}

void MonteCarlo::hedgingPAndL(PnlMat *path, int H, double &prix, double &std_dev, double &ProfitLoss) 
{
    prix = 0;
    std_dev = 0;
    int N = opt_->nbTimeSteps_;
    double tau = opt_->T_ / (double) H;
    double e = exp(mod_->r_ * tau);
    
    PnlVect *delta_past = pnl_vect_create_from_zero(mod_->size_);
    PnlVect *delta_std_dev = pnl_vect_create_from_zero(mod_->size_);
    PnlVect *delta_current = pnl_vect_create_from_zero(mod_->size_);
    PnlVect *pathCurr = pnl_vect_create(mod_->size_);
    PnlMat *past = pnl_mat_create(1, mod_->size_);
    pnl_mat_get_row(pathCurr, path, 0);
    pnl_mat_set_row(past, pathCurr, 0);

    price(prix, std_dev);
    delta(delta_past, delta_std_dev);

    ProfitLoss = prix - pnl_vect_scalar_prod(delta_past, pathCurr);

    for (int i = 1; i < H+1; i++)
    {
        pnl_mat_get_row(pathCurr, path, i);
        if (i % (H / N) == 1)
        {
            pnl_mat_add_row(past, past->m, pathCurr);
        }
        else
        {
            pnl_mat_set_row(past, pathCurr, past->m-1);
        }
        
        delta(past, i * tau, delta_current, delta_std_dev);
        pnl_vect_minus_vect(delta_past, delta_current);
        ProfitLoss = ProfitLoss * e + pnl_vect_scalar_prod(delta_past, pathCurr);
        pnl_vect_clone(delta_past, delta_current); 
    }
    
    ProfitLoss += pnl_vect_scalar_prod(delta_current, pathCurr) - opt_->payoff(past);

    pnl_vect_free(&delta_past);
    pnl_vect_free(&delta_std_dev);
    pnl_vect_free(&delta_current);
    pnl_vect_free(&pathCurr);
    pnl_mat_free(&past);
}
