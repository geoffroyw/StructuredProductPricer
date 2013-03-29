/**
	Option de base
**/
#include "BaseOption.h"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>


/**
Par défaut, 52 pas de temps et 1000 trajectoires
**/
BaseOption::BaseOption(double riskFreeRate, double maturity)
{
	setRiskFreeRate(riskFreeRate);
	setMaturity(maturity);
	m_nbTimeStep = 52;
	m_nbSimulation = 1000;
}


BaseOption::~BaseOption(void)
{
}


void BaseOption::setNbTimeStep(int nbTimeStep) {
	if(nbTimeStep <=0) {
		throw domain_error("Nombre de pas de temps négatif");
	}
	m_nbTimeStep = nbTimeStep;
}

int BaseOption::getNbTimeStep() {
	return m_nbTimeStep;
}


void BaseOption::setRiskFreeRate(double riskFreeRate) {
	m_riskFreeRate = riskFreeRate;
}

double BaseOption::getRiskFreeRate() {
	return m_riskFreeRate;
}

void BaseOption::setMaturity(double maturity) {
	if(maturity<=0) {
		throw domain_error("Maturité négative");
	}
	m_maturity = maturity;
}

double BaseOption::getMaturity() {
	return m_maturity;
}

void BaseOption::setStrike(double strike) {
	if(strike<=0) {
		throw domain_error("Strike négatif");
	}
	m_strike = strike;
}

double BaseOption::getMaturity() {
	return m_strike;
}


void BaseOption::addUnderlyingAsset(Asset a) {
	m_underlyingAssets[m_underlyingAssets.count()+1] = a;
}


void BaseOption::setCorrelationMatrix(boost::numeric::ublas::matrix<double> correlationMatrix) {
	m_correlationMatrix = correlationMatrix;
}
boost::numeric::ublas::matrix<double> BaseOption::getCorrelationMatrix() {
	return m_correlationMatrix;
}
