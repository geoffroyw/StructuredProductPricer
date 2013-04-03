#include "BaseProduct.h"


BaseProduct::BaseProduct(void)
{
	riskFreeRate = 0.3;
	
	mConfidenceLowerBound = 0.0;
	mConfidenceUpperBound = 0.0;
	mPrice = 0.0;
	delta = 0.0;
	gamma = 0.0;
	theta = 0.0;
	vega = 0.0;
	rho = 0.0;

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
	maturity = m;
}
void BaseProduct::setNbTimestep(double nb) {
	nbTimestep =nb;
}
void BaseProduct::setNbSimulation(double nb) {
	nbSimulation = nb;
}
void BaseProduct::setRiskFreeRate(double r) {
	riskFreeRate = r;
}