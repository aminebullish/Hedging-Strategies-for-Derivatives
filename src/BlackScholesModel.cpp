#include "BlackScholesModel.hpp"

#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot)
{
    size_ = size;
    r_ = r;
    rho_ = rho;
    sigma_ = sigma;
    spot_ = spot;

    corr = pnl_mat_create_from_scalar(size_, size_, rho_);
    for(int k = 0; k < size_; k++)
	{
      pnl_mat_set(corr, k, k, 1);
    }
    pnl_mat_chol(corr);

    G = pnl_vect_create(size_);
	Ld = pnl_vect_create(size_);
}

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot, PnlVect *trend)
{
    size_ = size;
    r_ = r;
    rho_ = rho;
    sigma_ = sigma;
    spot_ = spot;
    trend_ = trend;

    corr = pnl_mat_create_from_scalar(size_, size_, rho_);
    for(int k = 0; k < size_; k++)
	{
      pnl_mat_set(corr, k, k, 1);
    }
    pnl_mat_chol(corr);

    G = pnl_vect_create(size_);
	Ld = pnl_vect_create(size_);
}


BlackScholesModel::~BlackScholesModel()
{
    pnl_mat_free(&corr);
    pnl_vect_free(&G);
	pnl_vect_free(&Ld);
}

void BlackScholesModel::asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng)
{
	double sigma_d, x, prod_scal;
    double step = T / (double) nbTimeSteps;
    double sq_step = sqrt(step);

    pnl_mat_set_row(path, spot_, 0);

	for (int i = 1; i < nbTimeSteps + 1; i++)
	{
		pnl_vect_rng_normal(G, size_, rng);
		for (int d = 0; d < size_; d++)
		{
			pnl_mat_get_row(Ld, corr, d);
            prod_scal = pnl_vect_scalar_prod(Ld, G);
			sigma_d = GET(sigma_, d);
			MLET(path, i, d) = MGET(path, i-1, d) * exp((r_ - sigma_d * sigma_d / 2.) * step 
                                                        + sigma_d * sq_step * prod_scal);
		}
	}
}

void BlackScholesModel::asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past)
{
    double sigma_d, sq_timeInterval, prod_scal;
    double N = (double) nbTimeSteps;
    double step = T / N;
    int lastDatePast = (int) floor(t / step);
    double timeInterval = (lastDatePast + 1) * step - t;

    pnl_mat_set_subblock(path, past, 0, 0);
    
    for (int i = lastDatePast + 1; i < nbTimeSteps + 1; i++) 
    {
        sq_timeInterval = sqrt(timeInterval);
        pnl_vect_rng_normal(G, size_, rng);
        for (int d = 0; d < size_; d++) 
        {
            pnl_mat_get_row(Ld, corr, d);
            prod_scal = pnl_vect_scalar_prod(Ld, G);
            sigma_d = GET(sigma_, d);
            MLET(path, i, d) = MGET(past, past->m-1, d)
                               * exp((r_ - sigma_d * sigma_d / 2.) * timeInterval
                                      + sigma_d * sq_timeInterval * prod_scal);
        }
        timeInterval += step;
    }
}

void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep)
{
    int nextDatePast = (int) ceil(t / timestep);
    pnl_mat_clone(shift_path, path);
    for (int i = nextDatePast; i < path->m; i++)
    {
        MLET(shift_path, i, d) *= h + 1.;
    }
}

void BlackScholesModel::simul_market(PnlMat *path, double T, int H, PnlRng *rng)
{
    double sigma_d;
    double step = T / (double) H;
    double sq_step = sqrt(step);

    pnl_mat_set_row(path, spot_, 0);

	for (int i = 1; i < H + 1; i++)
	{
		pnl_vect_rng_normal(G, size_, rng);
		for (int d = 0; d < size_; d++)
		{
			pnl_mat_get_row(Ld, corr, d);
			sigma_d = GET(sigma_, d);
			MLET(path, i, d) = MGET(path, i-1, d) * exp((GET(trend_, d) - sigma_d * sigma_d / 2.) * step + 
                                                        sigma_d * sq_step *pnl_vect_scalar_prod(Ld, G));
		}
	}
}


