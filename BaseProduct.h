#ifndef BASEPRODUCT_H
#define BASEPRODUCT_H
#include <vector>

class BaseProduct
{
protected:
	double maturity;
	int nbTimestep;
	int nbSimulation;
	double mConfidenceLowerBound;
	double mConfidenceUpperBound;
	double mPrice;
	std::vector<double> mRandVars;
	double spotPrice;
	double delta;
	double gamma;
	double theta;
	double rho;
	double vega;
	double riskFreeRate;
	
public:
	BaseProduct(void);
	~BaseProduct(void);
	double getConfidenceLowerBound();
	double getConfidenceUpperBound();
	virtual void price();
	virtual void computeGreeks();
	double getPrice();
	double getDelta();
	double getGamma();
	double getTheta();
	double getRho();
	double getVega();
	void setMaturity(double m);
	void setNbTimestep(double nb);
	void setNbSimulation(double nb);
	void setRiskFreeRate(double r);
};

#endif