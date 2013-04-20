#include "BaseProduct.h"
#include <stdexcept>
#include <cmath>
#include "SimulationType.h"

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
	simulationType = SimulationType::MonteCarlo;
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
void BaseProduct::simulateRandVars(){}

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

void BaseProduct::setSimulationType(SimulationType s) {
	simulationType = s;
}

SimulationType BaseProduct::getSimulationType() {
	return simulationType;
}

double BaseProduct::FaureBase2(int n) {
	double f(0.0), sb(0.5);
	int i, n1, n2;
	n1 = n;
	do {
		n2 = (int)n1/2;
		i = n1-n2*2;
		f += sb*i;
		sb /=2;
		n1 = n2;
	}while(n1>0);

	return moro_normSInv(f);
}

// Calculate the Normal Standard numbers given u, the associated uniform number (0,1) 
double BaseProduct::moro_normSInv(double u) {
	double c1, c2, c3, c4, c5, c6, c7, c8, c9;
	double x, r;
	double a[4]; 
	a[0] = 2.50662823884;
	a[1] = -18.61500062529;
	a[2] = 41.39119773534;
	a[3] -25.44106049637;
	double b[4];
	b[0] = -8.4735109309;
	b[1] = 23.08336743743;
	b[2] = -21.06224101826;
	b[3] = 3.13082909833;

	c1 = 0.337475482272615;
	c2 = 0.976169019091719;
	c3 = 2.76438810333863E-02;
	c4 = 2.76438810333863E-02;
    c5 = 3.8405729373609E-03;
    c6 = 3.951896511919E-04;
    c7 = 3.21767881768E-05;
    c8 = 2.888167364E-07;
    c9 = 3.960315187E-07;

	x = u - 0.5;

	if(std::abs(x) < 0.42) {
		r = x*x;
		r = x * (((a[3] * r + a[2]) * r + a[1]) * r + a[0]) / ((((b[3]* r + b[2]) * r + b[1]) * r + b[0]) * r + 1);
	}
	else {
		if(x>0) {
			r = log(-log(1-u));
		}
		if(x<=0) {
			r = log(-log(u));
		}
		r = c1 + r * (c2 + r * (c3 + r * (c4 + r * (c5 + r * (c6 + r * (c7 + r * (c8 + r * c9)))))));
		if(x<=0) {
			r = -r;
		}
	}
	return r;
}