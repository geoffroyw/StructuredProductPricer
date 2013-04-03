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
	double payoff;
	double sum_payoffs = 0.0;
	double delta_t = maturity/nbTimestep;
	double r;
	double sd;
	double S_t;

	vector<double> payoffs;
	
	bool low_barrier_reach = false;
	bool high_barrier_reach = true;

	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);

	for(int k=0;k<nbSimulation;k++) {
		high_barrier_reach = false;
		low_barrier_reach = false;


		//On simule les trajectoires
		r = (riskFreeRate-0.5*pow(vol,2))*delta_t;
		sd = vol*sqrt(delta_t);
		S_t = spotPrice;
		for(int j = 1; j<nbTimestep;j++) {
			double rnd = var_nor();
			mRandVars.push_back(rnd);
			S_t= S_t*exp(r+sd*rnd);

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