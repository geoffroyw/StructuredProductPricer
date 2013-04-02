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
}

void BonusCliquet::price() {
	double maturity = 7.0;
	double performance_obj = 0.35;
	int nbTimeStep = 52*maturity;
	int nbSimulation = 1000;
	double sum_payoffs(0);
	double riskFreeRate = 0.03;
	double spotPrice = 100.0;
	double low_barrier = 80;
	double high_barrier = 120;
	double vol = 0.3;
	vector<double> payoffs;
	double payoff;


	bool low_barrier_reach = false;
	bool high_barrier_reach = true;

	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);


	double delta_t = maturity/nbTimeStep;
	double r;
	double sd;
	double S_t;

	for(int k=0;k<nbSimulation;k++) {
		high_barrier_reach = false;
		low_barrier_reach = false;


		//On simule les trajectoires
		r = (riskFreeRate-0.5*pow(vol,2))*delta_t;
		sd = vol*sqrt(delta_t);
		S_t = spotPrice;
		for(int j = 1; j<nbTimeStep;j++) {
			S_t= S_t*exp(r+sd*var_nor());

			if(S_t>=high_barrier) {
				high_barrier_reach = true;
			}
			if(S_t<=low_barrier) {
				low_barrier_reach = true;
			}
		}

		payoff = 0;
		if(!low_barrier_reach) {
			if(S_t>high_barrier) {
				payoff=S_t;
			}
			else {
				payoff=high_barrier;
			}
		}
		else {
			if(high_barrier_reach) {
				payoff = high_barrier;
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