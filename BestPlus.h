#ifndef BESTPLUS_H
#define BESTPLUS_H
#include "BaseProduct.h"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
class BestPlus : public BaseProduct 
{
private:
	int nbAsj;
	std::vector<double> spotPrices;
	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> correlations;
	double performance_obj;
	double capital;
	boost::numeric::ublas::matrix<double> cholM;
	void cholesky();
public:
	BestPlus(void);
	~BestPlus(void);
	virtual void price();
	virtual void computeGreeks();
	virtual void simulatePaths();
	virtual void simulateRandVars();
	void setNbAsj(int nb);
	void setSpotPrices(std::vector<double> spotPrices);
	void addSpotPrice(double spotPrice);
	void setCorrelations(boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> corr);
	void setPerformanceObj(double obj);
	void setCapital(double c);
	

};



#endif