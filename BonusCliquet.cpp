/**
	Pricing d'un Bonus Cliquet
*/

#include <stdio.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include "BonusCliquet.h"
using namespace std;

BonusCliquet::BonusCliquet(void) : BaseProduct()
{
	maturity = 7.0;
	nbTimestep = 52*(int)maturity;
	nbSimulation = 1000;
	spotPrice = 100.0;
	lowBarrier = 80;
	highBarrier = 120;
	vol = 0.3;
}

void BonusCliquet::price() {
	simulateRandVars();
	simulatePaths();
	return;

}

void BonusCliquet::simulatePaths() {
	int randVarIndex = 0;

	double payoff;
	double sum_payoffs = 0.0;
	double delta_t = maturity/nbTimestep;
	double r = (riskFreeRate-0.5*pow(vol,2))*delta_t;
	double sd = vol*sqrt(delta_t);
	double S_t;

	vector<double> payoffs;
	
	bool low_barrier_reach = false;
	bool high_barrier_reach = true;

	//Simulation
	for(int k=0;k<nbSimulation;k++) {
		high_barrier_reach = false;
		low_barrier_reach = false;
		S_t = spotPrice;

		//On simule les trajectoires
		for(int j = 1; j<nbTimestep;j++) {
			S_t= S_t*exp(r+sd*mRandVars[randVarIndex++]);

			if(S_t>=highBarrier) {
				high_barrier_reach = true;
			}
			if(S_t<=lowBarrier) {
				low_barrier_reach = true;
			}
		}

		payoff = 0.0;

		if(!low_barrier_reach) {
			if(S_t>highBarrier) {
				payoff=S_t;
			}
			else {
				payoff=highBarrier;
			}
		}
		else {
			if(high_barrier_reach) {
				payoff = highBarrier;
			}
			else {
				payoff=S_t;
			}
		}

		payoff*=exp(-riskFreeRate*maturity);
		sum_payoffs+=payoff;
		payoffs.push_back(payoff);
	}

	mPrice = sum_payoffs/nbSimulation;

	//Variance des payoffs
	double var = 0;
	for(int i=0;i<nbSimulation;i++){
		var+=pow((payoffs[i]-mPrice),2);
	}
	var/=(nbSimulation-1);
	var=sqrt(var);

	mConfidenceLowerBound = mPrice-2*var/sqrt(nbSimulation);
	mConfidenceUpperBound = mPrice+2*var/sqrt(nbSimulation);
	return;
}


//Simulate random variables
void BonusCliquet::simulateRandVars() {
	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
	for(int k=0;k<nbSimulation;k++) {
		for(int j = 1; j<nbTimestep;j++) {
			mRandVars.push_back(var_nor());
		}
	}
}


//Calcul des Grecques par différence divisées
void BonusCliquet::computeGreeks() {
	double s1,s,s2;
	simulatePaths();
	s = mPrice;
	setSpotPrice(spotPrice-deltaS);
	simulatePaths();
	s1 = mPrice;
	setSpotPrice(spotPrice+2*deltaS);
	simulatePaths();
	s2 = mPrice;

	delta = (s2-s1)/(2*deltaS);
	gamma = (s2 - 2*s+s1)/pow(deltaS,2);

	setSpotPrice(spotPrice-deltaS);

	setMaturity(maturity-deltaT);
	simulatePaths();
	s1 = mPrice;
	setMaturity(maturity+2*deltaT);
	simulatePaths();
	s2 = mPrice;
	theta = -(s2-s1)/(2*deltaT);

	setMaturity(maturity-deltaT);

	
	setRiskFreeRate(riskFreeRate-deltaR);
	simulatePaths();
	s1 = mPrice;
	setRiskFreeRate(riskFreeRate+2*deltaR);
	simulatePaths();
	s2 = mPrice;
	rho = (s2-s1)/(2*deltaR);

	setRiskFreeRate(riskFreeRate-deltaR);

	setVol(vol-deltaSigma);
	simulatePaths();
	s1 = mPrice;
	setVol(vol+2*deltaSigma);
	simulatePaths();
	s2 = mPrice;
	vega = (s2-s1)/(2*deltaSigma);
	setVol(vol-deltaSigma);
}



void BonusCliquet::setSpotPrice(double price) {
	spotPrice = price;
}

void BonusCliquet::setLowBarrier(double b) {
	lowBarrier=b;
}

void BonusCliquet::setHighBarrier(double b) {
	highBarrier = b;
}

void BonusCliquet::setVol(double v) {
	vol = v;
}

void BonusCliquet::setMaturity(double m) {
	BaseProduct::setMaturity(m);
	nbTimestep = 52*(int)maturity;
}