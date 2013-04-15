#include "BaseProduct.h"
#include <stdexcept>

BaseProduct::BaseProduct(void)
{

	mConfidenceLowerBound = 0.0;
	mConfidenceUpperBound = 0.0;
	mPrice = 0.0;
	delta = 0.0;
	gamma = 0.0;
	theta = 0.0;
	vega = 0.0;
	rho = 0.0;

	setRiskFreeRate(0.03);
	setDeltaS(0.001);
	setDeltaR(0.001);
	setDeltaSigma(0.001);
	setDeltaT(0.001);

}


BaseProduct::~BaseProduct(void)
{
}

double BaseProduct::getConfidenceLowerBound() {
	return mConfidenceLowerBound;
}
double BaseProduct::getConfidenceUpperBound() {
	return mConfidenceUpperBound;
}
void BaseProduct::price(){}
void BaseProduct::computeGreeks() {}
void BaseProduct::simulatePaths(){}
void BaseProduct::simulateRandVars(){};

double BaseProduct::getPrice() {
	return mPrice;
}

double BaseProduct::getDelta() {
	return delta;
}
double BaseProduct::getGamma() {
	return gamma;
}
double BaseProduct::getRho() {
	return rho;
}
double BaseProduct::getTheta() {
	return theta;
}
double BaseProduct::getVega() {
	return vega;
}

void BaseProduct::setMaturity(double m) {
	if(m<=0) {
		throw std::out_of_range("La maturité doit être strictement positive");
	}
	else {
		maturity = m;
	}
}
void BaseProduct::setNbTimestep(int nb) {
	if(nb<=0) {
		throw std::out_of_range("Le nombre de pas de temps doit être strictement positif");
	}
	else {
		nbTimestep =nb;
	}
}
void BaseProduct::setNbSimulation(int nb) {
	if(nb<=0) {
		throw std::out_of_range("Le nombre de simulation doit être strictement positif");
	}
	else {
		nbSimulation = nb;
	}
}
void BaseProduct::setRiskFreeRate(double r) {
	riskFreeRate = r;
}

void BaseProduct::setDeltaS(double ds){
	if(ds==0) {
		throw std::invalid_argument("ds doit être différent de zéro");
	}
	else {
		deltaS=std::abs(ds);
	}
}

void BaseProduct::setDeltaT(double dt){
	if(dt==0) {
		throw std::invalid_argument("dt doit être différent de zéro");
	}
	else {
		deltaT = std::abs(dt);
	}
}
void BaseProduct::setDeltaR(double dr){
	if(dr==0) {
		throw std::invalid_argument("dr doit être différent de zéro");
	}
	else {
		deltaR = std::abs(dr);
	}
}
void BaseProduct::setDeltaSigma(double ds){
	if(ds==0) {
		throw std::invalid_argument("dsigma doit être différent de zéro");
	}
	else {
		deltaSigma = std::abs(ds);
	}
}