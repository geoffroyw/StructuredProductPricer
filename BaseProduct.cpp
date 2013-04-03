#include "BaseProduct.h"


BaseProduct::BaseProduct(void)
{
	riskFreeRate = 0.03;
	
	mConfidenceLowerBound = 0.0;
	mConfidenceUpperBound = 0.0;
	mPrice = 0.0;
	delta = 0.0;
	gamma = 0.0;
	theta = 0.0;
	vega = 0.0;
	rho = 0.0;

	deltaS = 0.001;
	deltaR = 0.001;
	deltaSigma = 0.001;
	deltaT = 0.001;

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
	maturity = m;
}
void BaseProduct::setNbTimestep(int nb) {
	nbTimestep =nb;
}
void BaseProduct::setNbSimulation(int nb) {
	nbSimulation = nb;
}
void BaseProduct::setRiskFreeRate(double r) {
	riskFreeRate = r;
}

void BaseProduct::setDeltaS(double ds){
	deltaS=ds;
}

void BaseProduct::setDeltaT(double dt){
	deltaT = dt;
}
void BaseProduct::setDeltaR(double dr){
	deltaR = dr;
}
void BaseProduct::setDeltaSigma(double ds){
	deltaSigma = ds;
}