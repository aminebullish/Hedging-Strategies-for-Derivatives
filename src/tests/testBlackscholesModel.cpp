#include "../BlackScholesModel.hpp"
#include <iostream>

int main(){
    int nbTimesteps = 3;
    int size = 4;
    double vol = 0.25;
    double initspot = 5;
    double r = 0.005;
    double T = 36;
    PnlVect *sigma = pnl_vect_create_from_scalar(size,vol);
    PnlVect *spot = pnl_vect_create_from_scalar(size,initspot);
    BlackScholesModel bs = BlackScholesModel(size, r, vol, sigma, spot);
    PnlMat *path = pnl_mat_create_from_zero(nbTimesteps + 1, size);
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    bs.asset(path, T, nbTimesteps, rng);
    for(int i=0; i<size; i++){
        for(int j=0; j<nbTimesteps + 1; j++){
            printf("%f         ",pnl_mat_get(path, j, i));
        }
        printf("\n");
    }

    printf("//////////////////////test asset pout tout t///////////////////\n");

    PnlMat *pnlMat = pnl_mat_create_from_zero(nbTimesteps + 1, size);
    double t = 3;
    PnlMat *past = pnl_mat_create_from_scalar(t, size, 4);
    bs.asset(pnlMat, t, T, nbTimesteps, rng, past);
    for(int i=0; i<size; i++){
        for(int j=0; j<nbTimesteps + 1; j++){
            printf("%f         ",pnl_mat_get(pnlMat, j, i));
        }
        printf("\n");
    }

    PnlMat *shift_path = pnl_mat_create_from_zero(nbTimesteps + 1, size);
    bs.shiftAsset(shift_path, path, 3, 0.5, 0.0, 2.0);
    printf("//////////////////////test shift path///////////////////\n");
    for(int i=0; i<size; i++){
        for(int j=0; j<nbTimesteps + 1; j++){
            printf("%f         ",pnl_mat_get(shift_path, j, i));
        }
        printf("\n");
    }

    printf("///////////////////////simul_market test////////////////////");
    int H = 12;
    PnlVect *trends = pnl_vect_create_from_scalar(size, 0.006);
    BlackScholesModel bs_s = BlackScholesModel(size, r, vol, sigma, spot, trends);
    PnlMat *path_s = pnl_mat_create_from_zero(H+1, size);
    bs_s.simul_market(path_s, T, H, rng );
    printf("\n");
    for(int i=0; i<size; i++){
        for(int j=0; j<H ; j++){
            printf("%f   ",pnl_mat_get(path_s, j, i));
        }
        printf("\n");
    }

    
    pnl_mat_free(&shift_path);
    pnl_mat_free(&pnlMat);
    pnl_mat_free(&past);
	pnl_vect_free(&sigma);
    pnl_vect_free(&spot);
    pnl_mat_free(&path);
    pnl_rng_free(&rng);
    pnl_vect_free(&trends);
    pnl_mat_free(&path_s);

    std::cout << "\nTest Passed\n" << std::endl;
    return 0;
}
