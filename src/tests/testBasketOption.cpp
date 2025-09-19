#include "../BasketOption.hpp"
#include <iostream>

int main(){
    double T_ = 20;
    int size_ = 4;
    double strike = 5;
    int nbTimeSteps = 5;
    PnlVect *lambdas = pnl_vect_create_from_scalar(size_, 0.25);
    Basket bas = Basket(T_, nbTimeSteps, size_, strike, lambdas);
    PnlMat *path = pnl_mat_create_from_scalar(nbTimeSteps+1, size_, 7);
    std::cout << " Test Passed " << bas.payoff(path)<< std::endl;
    pnl_vect_free(&lambdas);
    pnl_mat_free(&path);
    return 0;
}