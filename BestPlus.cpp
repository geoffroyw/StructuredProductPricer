/**
  Pricer d'un BestPlus 49
*/

#include <stdio.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include "BestPlus.h"


using namespace std;


BestPlus::BestPlus(void) : BaseProduct()
{
	maturity = 7.0;
	nbTimestep = 7;
	nbSimulation = 1000;
	nbAsj = 14;
	performance_obj = 0.35;
	capital = 100.0;

	correlations.resize(nbAsj,nbAsj,false);
	cholM.resize(nbAsj,nbAsj,false);

	//On initialise tous les prix spots à 100
	for(int i =0;i<nbAsj;i++) {
		spotPrices.push_back(100.0);
	}

	// Initialisation de la matriec des corrélations
	for(unsigned i=0;i<correlations.size1();++i) {
		for(unsigned j=0;j<i;++j) {
			correlations(i,j) = 0.05+i/(j+1);
		}
		correlations(i,i)=0.3;
	}

}

void BestPlus::price() {
	int timestep_first_coupon = -1;

	double payoff = 0.0;
	double sum_payoffs = 0.0;
	double best_pf_performance = 0.0;
	double delta_t = maturity/nbTimestep;
	double r;
	double sd;
	double vol;
	double temp;

	vector<double> spot_prices(nbAsj);
	vector<double> saved_pf_value(nbTimestep);
	vector<double> payoffs(nbSimulation);
	vector<double> randVars(nbAsj);

	boost::numeric::ublas::matrix<double> S_ts(nbAsj,nbTimestep);
	
	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);


	//Première valeur sauvegardée du portefeuille :  moyenne des spots
	for(int i=0;i<spotPrices.size();i++) {
		saved_pf_value[0]+=spotPrices[i];
	}
	saved_pf_value[0]/=nbAsj;


	//Décomposition de Cholesky de la matrice des corrélations
	cholesky();

	//Simulation
	for(int k = 0;k<nbSimulation;k++) {
		
		///vector<double> vols;
		boost::numeric::ublas::matrix<double> vols(nbAsj,nbTimestep);

		//On génère des vars indépendantes distribuées normalement
		for(int p = 0;p<nbTimestep;p++) {
			for(int i = 0; i<nbAsj; i++) {
				randVars.push_back(var_nor());
			}
		
			for(int i=0;i<nbAsj;i++) {
				temp=0;
				for(int j=0;j<=i;j++) {
					temp+=randVars[j]*cholM(i,j);
				}
				mRandVars.push_back(temp);
				//vols.push_back(temp);
				vols(i,p) = temp;
			}
		}
		
		//Simulation des trajectoires
		for(int i = 0; i<nbAsj;i++) {
			vol = correlations(i,i);
			r = (riskFreeRate-0.5*pow(vol,2))*delta_t;
			sd = vol*sqrt(delta_t);
			S_ts(i,0) = spot_prices[i];

			for(int j = 1; j<nbTimestep;j++) {
				//S_ts(i,j)= S_ts(i,j-1)*exp(r+sd*vols[i]);
				S_ts(i,j)= S_ts(i,j-1)*exp(r+sd*vols(i,j));
			}
		}

		boost::numeric::ublas::matrix<double> performance(nbAsj,nbTimestep);
		vector<double> saved_performance_index(nbAsj);
		
		int best_perf1_index(1), best_perf2_index(0);
		double best_perf1_value(1), best_perf2_value(0);
		//A chaque date anniversaire, on prend les deux meilleures performances
		for(int j=1;j<nbTimestep;j++) {
			for(int i = 0;i<nbAsj;i++) {
				if(find(saved_performance_index.begin(), saved_performance_index.end(), i) == saved_performance_index.end()) {
					performance(i,j)=(S_ts(i,j)-S_ts(i,j-1))/S_ts(i,j-1);
					
					if(performance(i,j)>=best_perf1_value) {
						best_perf2_value = best_perf1_value;
						best_perf2_index = best_perf1_index;
						best_perf1_value = performance(i,j);
						best_perf1_index = i; 
					}
					if(performance(i,j)>best_perf2_value) {
						best_perf2_value = performance(i,j);
						best_perf2_index = i; 
					}
				}
			}

			saved_performance_index.push_back(best_perf1_value);
			saved_performance_index.push_back(best_perf2_value);
			
			for(int l=j+1;l<nbTimestep-1;l++) {
				performance(best_perf1_index,l)=performance(best_perf1_index,j);
				performance(best_perf2_index,l)=performance(best_perf2_index,j);
			}

			//Calcul de la valeur du PF sécurisée
			saved_pf_value[j]=0;
			for(int i=0;i<nbAsj;i++) {
				saved_pf_value[j]+=S_ts(i,j);
			}
			saved_pf_value[j]/=nbAsj;

			if(best_pf_performance<(saved_pf_value[j]-saved_pf_value[j-1])/saved_pf_value[j-1]) {
				best_pf_performance = (saved_pf_value[j]-saved_pf_value[j-1])/saved_pf_value[j-1];
			}

			//Est-ce qu'on donne un coupon ?
			if(best_pf_performance>=performance_obj && timestep_first_coupon<0) {
				timestep_first_coupon = j;
			}

		}

		// Payoff = NPV des cash-flows
		double memory_coupon =0;
		payoff = 0;
		if(timestep_first_coupon>=0) {
			memory_coupon = timestep_first_coupon*0.07;
			payoff = capital*memory_coupon*exp(-riskFreeRate*timestep_first_coupon);

			for(int i = timestep_first_coupon; i<nbTimestep;i++) {
				payoff += capital*0.07*exp(-riskFreeRate*i);
			}
		}
		payoff+=capital*(1+best_pf_performance)*exp(-riskFreeRate*(nbTimestep));
		payoffs.push_back(payoff);

		sum_payoffs+=payoff;
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

// Décomposition de Cholesky
void BestPlus::cholesky() {
	double x;
	for(unsigned i=0;i<cholM.size1();++i) {
		for(unsigned j=0;j<cholM.size2();++j) {
			cholM(i,j)=0;
		}
	}
	for(int i=0;i<cholM.size1();++i) {
		for(int j=i;j<cholM.size2();++j) {
			x = correlations(i,j);
			for(int k=0;k<i-1;++k) {
				x = x-cholM(i,k)*cholM(j,k);
			}
			if(i==j) {
				cholM(i,j)=sqrt(x);
			}
			else {
				cholM(j,i)=x/cholM(i,i);
			}
		}
	}
}


void BestPlus::computeGreeks(){

}

void BestPlus::setSpotPrices(vector<double> sps) {
	spotPrices = sps;
}
void BestPlus::addSpotPrice(double sp) {
	spotPrices.push_back(sp);
}
void BestPlus::setCorrelations(boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> corr) {
	correlations = corr;
}
void BestPlus::setPerformanceObj(double obj) {
	performance_obj = obj;
}
void BestPlus::setCapital(double c) {
	capital = c;
}


