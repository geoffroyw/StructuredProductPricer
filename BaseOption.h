#ifndef BASEOPTION_H
#define BASEOPTION_H

#include "Asset.h"
#include <map>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>


class BaseOption
{
private:
	std::map<int,Asset> m_underlyingAssets;
	boost::numeric::ublas::matrix<double> m_correlationMatrix;
	int m_nbTimeStep;
	int m_nbSimulation;
	double m_maturity;
	double m_riskFreeRate;
	double m_price;
	double m_strike;


public:
	BaseOption(double riskFreeRate, double maturity);

	void setNbSimulation(int nbSimulation);
	int getNbSimulation();
	void setNbTimeStep(int nbTimeStep);
	int getNbTimeStep();
	void setRiskFreeRate(double riskFreeRate);
	double getRiskFreeRate();
	void setMaturity(double riskFreeRate);
	double getMaturity();
	void setStrike(double strike);
	double getStrike();
	void setCorrelationMatrix(boost::numeric::ublas::matrix<double> m_correlationMatrix);
	boost::numeric::ublas::matrix<double> getCorrelationMatrix();

	void addUnderlyingAsset(Asset a);

	virtual double getPrice();

	~BaseOption(void);
};

#endif