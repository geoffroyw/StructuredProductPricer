/**
  Pricer d'un BestPlus 49
*/

#include <stdio.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include "BestPlus.h"


using namespace std;


BestPlus::BestPlus(void) : BasketProduct()
{
	maturity = 7.0;
	nbTimestep = 7;
	nbSimulation = 1000;
	nbAsj = 14;
	performance_obj = 0.35;
	capital = 100.0;

	correlations.resize(nbAsj,nbAsj,false);
	cholM.resize(nbAsj,nbAsj,false);

	//On initialise tous les prix spots � 100
	for(int i =0;i<nbAsj;i++) {
		spotPrices.push_back(100.0);
	}

	// Initialisation de la matriec des corr�lations
	for(unsigned i=0;i<correlations.size1();++i) {
		for(unsigned j=0;j<i;++j) {
			correlations(i,j) = 0.05;
		}
		correlations(i,i)=0.3;
	}

}

void BestPlus::price() {
	simulateRandVars();
	simulatePaths();
	return;
}


void BestPlus::simulatePaths() {
	int timestep_first_coupon = -1;
	int randVarIndex = 0;
	double payoff = 0.0;
	double sum_payoffs = 0.0;
	double best_pf_performance = 0.0;
	double delta_t = maturity/nbTimestep;
	double r;
	double sd;
	double vol;
	
	//vector<double> spot_prices(nbAsj);
	vector<double> saved_pf_value(nbTimestep);
	vector<double> payoffs(nbSimulation);
	vector<double> randVars(nbAsj);

	boost::numeric::ublas::matrix<double> S_ts(nbAsj,nbTimestep);

	//Premi�re valeur sauvegard�e du portefeuille :  moyenne des spots
	for(int i=0;i<spotPrices.size();i++) {
		saved_pf_value[0]+=spotPrices[i];
	}
	saved_pf_value[0]/=nbAsj;

	//Simulation des trajectoires
	for(int k = 0;k<nbSimulation;k++) {
		
		for(int i = 0; i<nbAsj;i++) {
			vol = correlations(i,i);
			r = (riskFreeRate-0.5*pow(vol,2))*delta_t;
			sd = vol*sqrt(delta_t);
			S_ts(i,0) = spotPrices[i];

			for(int j = 1; j<nbTimestep;j++) {
				S_ts(i,j)= S_ts(i,j-1)*exp(r+sd*mRandVars[randVarIndex++]);
			}
		}

		boost::numeric::ublas::matrix<double> performance(nbAsj,nbTimestep);
		vector<int> saved_performance_index;
		

		int best_perf1_index(1), best_perf2_index(0);
		double best_perf1_value(-1), best_perf2_value(-1);

		//A chaque date anniversaire, on prend les deux meilleures performances
		for(int j=1;j<nbTimestep;j++) {
			best_perf1_value = -1;
			best_perf2_value = -1;
			for(int i = 0;i<nbAsj;i++) {
				if(find(saved_performance_index.begin(), saved_performance_index.end(), i) == saved_performance_index.end()) {
					performance(i,j)=(S_ts(i,j)-S_ts(i,j-1))/S_ts(i,j-1);
					
					if(performance(i,j)>=best_perf1_value) {
						best_perf2_value = best_perf1_value;
						best_perf2_index = best_perf1_index;
						best_perf1_value = performance(i,j);
						best_perf1_index = i; 
					}
					if(performance(i,j)>best_perf2_value && i!=best_perf1_index) {
						best_perf2_value = performance(i,j);
						best_perf2_index = i; 
					}
				}
			}

			saved_performance_index.push_back(best_perf1_index);
			saved_performance_index.push_back(best_perf2_index);
			
			for(int l=j+1;l<nbTimestep-1;l++) {
				performance(best_perf1_index,l)=performance(best_perf1_index,j);
				performance(best_perf2_index,l)=performance(best_perf2_index,j);
			}

			//Calcul de la valeur du PF s�curis�e
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
		payoff+=capital*(1+best_pf_performance)*exp(-riskFreeRate*nbTimestep);
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

void BestPlus::simulateRandVars() {
	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
	double temp;
	
	cholesky();


	for(int k = 0;k<nbSimulation;k++) {
		for(int l =0; l<nbTimestep; l++) {
			vector<double> randVars;
			for(int i = 0; i<nbAsj; i++) {
				randVars.push_back(var_nor());
			}
			for(int i=0;i<nbAsj;i++) {
				temp=0.0;
				for(int j=0;j<=i;j++) {
					temp+=randVars[j]*cholM(i,j);
				}
				mRandVars.push_back(temp);
			}
		}
	}
}

void BestPlus::computeGreeks(){
	double s1,s,s2;
	vector<double> initialSpotPrices = spotPrices;
	vector<double> pSpotPrices(spotPrices.size());
	vector<double> mSpotPrices(spotPrices.size());
	for(int i=0;i<spotPrices.size();i++) {
		pSpotPrices[i]=initialSpotPrices[i]+deltaS;
		mSpotPrices[i]=initialSpotPrices[i]-deltaS;
	}

	simulatePaths();
	s = mPrice;
	setSpotPrices(mSpotPrices);
	simulatePaths();
	s1 = mPrice;
	setSpotPrices(pSpotPrices);
	simulatePaths();
	s2 = mPrice;

	delta = (s2-s1)/(2*deltaS);
	gamma = (s2 - 2*s+s1)/pow(deltaS,2);

	setSpotPrices(initialSpotPrices);

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

	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> initialCorrelations = correlations;
	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> mCorrelations = correlations;
	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> pCorrelations = correlations;
	for(int i =0; i<correlations.size1(); i++) {
		mCorrelations(i,i)=initialCorrelations(i,i)-deltaSigma;
		pCorrelations(i,i)=initialCorrelations(i,i)+deltaSigma;
	}


	setCorrelations(mCorrelations);
	simulateRandVars();
	simulatePaths();
	s1 = mPrice;
	setCorrelations(pCorrelations);
	simulateRandVars();
	simulatePaths();
	s2 = mPrice;
	vega = (s2-s1)/(2*deltaSigma);
	setCorrelations(initialCorrelations);
	simulateRandVars();
}

void BestPlus::setPerformanceObj(double obj) {
	performance_obj = obj;
}
void BestPlus::setCapital(double c) {
	capital = c;
}


