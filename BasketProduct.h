#ifndef BASKETPRODUCT_H
#define BASKETPRODUCT_H
#include "baseproduct.h"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>

class BasketProduct :
	public BaseProduct
{
protected:
	int nbAsj;
	std::vector<double> spotPrices;
	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> correlations;
	boost::numeric::ublas::matrix<double> cholM;
	void cholesky();

public:
	BasketProduct(void);
	virtual ~BasketProduct(void);
	virtual void price()=0;
	virtual void computeGreeks()=0;
	virtual void simulatePaths()=0;
	virtual void simulateRandVars();
	virtual void simulatePseudoRandVars();
	void setNbAsj(int nb);
	void setSpotPrices(std::vector<double> spotPrices);
	void addSpotPrice(double spotPrice);
	void setCorrelations(boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> corr);
};

#endif
