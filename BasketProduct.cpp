#include "BasketProduct.h"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <stdexcept>
#include <ctime>

using namespace std;

BasketProduct::BasketProduct(void) : BaseProduct()
{
}


BasketProduct::~BasketProduct(void)
{
}

// Décomposition de Cholesky
void BasketProduct::cholesky() {
	double x;
	for(unsigned i=0;i<cholM.size1();++i) {
		for(unsigned j=0;j<cholM.size2();++j) {
			cholM(i,j)=0;
		}
	}
	for(int i=0;i<cholM.size1();i++) {
		for(int j=i;j<cholM.size2();j++) {
			x = correlations(i,j);
			for(int k=0;k<i-1;k++) {
				x = x-cholM(i,k)*cholM(j,k);
			}
			if(i==j) {
				cholM(i,i)=sqrt(x);
			}
			else {
				cholM(j,i)=x/cholM(i,i);
			}
		}
	}
}

void BasketProduct::simulateRandVars() {
	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
	double temp;
	
	cholesky();
	vector<double> randVars;

	for(int k = 0;k<nbSimulation;k++) {
		for(int l =0; l<nbTimestep; l++) {
			for(int i = 0; i<nbAsj; i++) {
				//randVars.push_back(var_nor());
				randVars.push_back(varNorm());
				temp = 0.0;
				for(int j=0;j<=i;j++) {
					temp+=randVars[j]*cholM(i,j);
				}
				mRandVars.push_back(temp);
			}
			randVars.clear();
		}
	}
	return;
}



void BasketProduct::simulatePseudoRandVars() {
	double temp;
	int randVarIndex = 1;

	cholesky();
	vector<double> randVars;

	for(int k = 0;k<nbSimulation;k++) {
		for(int l =0; l<nbTimestep; l++) {
			for(int i = 0; i<nbAsj; i++) {
				randVars.push_back(FaureBase2(randVarIndex++));
				temp = 0.0;
				for(int j=0;j<=i;j++) {
					temp+=randVars[j]*cholM(i,j);
				}
				mRandVars.push_back(temp);
			}
			randVars.clear();
		}
	}
	return;
}


void BasketProduct::setSpotPrices(vector<double> sps) {
	for(int i = 0; i<sps.size(); i++) {
		if(sps[i]<=0) {
			throw out_of_range("Les prix spots doivent être strictement positifs");
		}
	}
	spotPrices = sps;
}
void BasketProduct::addSpotPrice(double sp) {
	if(sp<=0) {
		throw out_of_range("Spot price doit être strictement positif");
	}
	else {
		spotPrices.push_back(sp);
	}
}
void BasketProduct::setCorrelations(boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> corr) {
	for(int i=0;i<corr.size1();i++) {
		for(int j=0;j<corr.size2();j++) {
			if(abs(corr(i,j))>1) {
				throw out_of_range("La corrélation doit être comprise entre -1 et +1");
			}
		}
	}
	correlations = corr;
}

void BasketProduct::setNbAsj(int nb) {
	if(nb<=0) {
		throw out_of_range("Le nombre d'ASJ doit être strictemenet positif");
	}
	else {
		nbAsj = nb;
	}
}