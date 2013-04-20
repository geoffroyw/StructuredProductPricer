/**
	Pricing d'un Bonus Cliquet
*/

#include <stdio.h>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include "BonusCliquet.h"
#include <stdexcept>
using namespace std;

BonusCliquet::BonusCliquet(void) : BaseProduct()
{
	setMaturity(7.0);
	setNbTimestep(52*(int)maturity);
	setNbSimulation(1000);
	setSpotPrice(100.0);
	setLowBarrier(80.0);
	setHighBarrier(120.0);
	setVol(0.3);
}
BonusCliquet::~BonusCliquet(void)
{
}

void BonusCliquet::price() {
	if(simulationType==SimulationType::MonteCarlo) {
		simulateRandVars();
	}
	else {
		simulatePseudoRandVars();
	}
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

	setMaturity(maturity+deltaT);
	simulateRandVars();
	setMaturity(maturity-deltaT);
	setMaturity(maturity-deltaT);
	
	simulatePaths();
	s1 = mPrice;
	setMaturity(maturity+2*deltaT);
	simulatePaths();
	s2 = mPrice;
	theta = -(s2-s1)/(2*deltaT);

	setMaturity(maturity-deltaT);
	simulateRandVars();

	
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

//Simulate pseudo random variables (Faure sequences)
void BonusCliquet::simulatePseudoRandVars() {
	int randVarIndex = 1;
	for(int k=0;k<nbSimulation;k++) {
		for(int j = 1; j<nbTimestep;j++) {
			mRandVars.push_back(FaureBase2(randVarIndex++));
		}
	}
}




void BonusCliquet::setSpotPrice(double price) {
	if(price<=0) {
		throw out_of_range("Le prix Spot doit être strictement positif");
	}
	else {
		spotPrice = price;
	}
}

void BonusCliquet::setLowBarrier(double b) {
	if(b<=0) {
		throw out_of_range("La barrière inférieure doit être strictement positive");
	}
	else {
		lowBarrier=b;
	}
}

void BonusCliquet::setHighBarrier(double b) {
	if(b<=0) {
		throw out_of_range("La barrière supérieure doit être strictement positive");
	}
	else {
		highBarrier = b;
	}
}

void BonusCliquet::setVol(double v) {
	if(v<0 || v>1) {
		throw out_of_range("La volatilité doit être comprise entre 0 et 1");
	}
	else {
		vol = v;
	}
}

void BonusCliquet::setMaturity(double m) {
	BaseProduct::setMaturity(m);
	setNbTimestep(52*(int)maturity);
}