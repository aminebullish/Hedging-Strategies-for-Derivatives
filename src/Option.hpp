#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/// \brief Classe Option abstraite
class Option
{
public:
    double T_; /// maturité
    int nbTimeSteps_; /// nombre de pas de temps de discrétisation
    int size_; /// dimension du modèle, redondant avec BlackScholesModel::size_
    double strike_;
    PnlVect *lambdas_;

    /**
     * Constructeur
     */
    Option(double T, int nbTimeSteps, int size, double strike, PnlVect *lambdas)
        : T_(T)
        , nbTimeSteps_(nbTimeSteps)
        , size_(size),strike_(strike)
        , lambdas_(lambdas)
    {}
    
    /**
     * Calcule la valeur du payoff sur la trajectoire
     *
     * @param[in] path est une matrice de taille (N+1) x d
     * contenant une trajectoire du modèle telle que créée
     * par la fonction asset.
     * @return phi(trajectoire)
     */
    virtual double payoff(const PnlMat *path) = 0;
};


