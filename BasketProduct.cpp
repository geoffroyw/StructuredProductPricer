#include "BasketProduct.h"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>

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

void BasketProduct::setSpotPrices(vector<double> sps) {
	spotPrices = sps;
}
void BasketProduct::addSpotPrice(double sp) {
	spotPrices.push_back(sp);
}
void BasketProduct::setCorrelations(boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> corr) {
	correlations = corr;
}

void BasketProduct::setNbAsj(int nb) {
	nbAsj = nb;
}