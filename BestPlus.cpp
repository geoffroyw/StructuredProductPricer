/**
  Pricer d'un BestPlus 49
*/

#include <stdio.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include "BestPlus.h"
#include <stdexcept>


using namespace std;


BestPlus::BestPlus(void) : BasketProduct()
{
	setMaturity(7.0);
	setNbTimestep(7);
	setNbSimulation(1000);
	setNbAsj(14);
	setPerformanceObj(0.35);
	setCapital(100.0);

	correlations.resize(nbAsj,nbAsj,false);
	cholM.resize(nbAsj,nbAsj,false);

	//On initialise tous les prix spots à 100
	for(int i =0;i<nbAsj;i++) {
		addSpotPrice(100.0);
	}

	// Initialisation de la matrice des corrélations
	for(unsigned i=0;i<correlations.size1();++i) {
		for(unsigned j=0;j<i;++j) {
			correlations(i,j) = 0.05;
		}
		correlations(i,i)=0.3;
	}

}
BestPlus::~BestPlus(void)
{
}

void BestPlus::price() {
	if(simulationType == SimulationType::MonteCarlo || simulationType == SimulationType::VarAntithetique) {
		simulateRandVars();
	}
	else {
		simulatePseudoRandVars();
	}
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
	
	vector<double> saved_pf_value(nbTimestep);
	vector<double> payoffs(nbSimulation);

	boost::numeric::ublas::matrix<double> S_ts(nbAsj,nbTimestep);

	//Première valeur sauvegardée du portefeuille :  moyenne des spots
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

void BestPlus::computeGreeks(){
	double s1,s,s2;

	delta = 0.0;
	gamma = 0.0;
	vega = 0.0;
	vector<double> initialSpotPrices = spotPrices;
	vector<double> pSpotPrices = spotPrices;
	vector<double> mSpotPrices = spotPrices;
	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> initialCorrelations = correlations;
	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> mCorrelations = correlations;
	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> pCorrelations = correlations;


	for(int k = 0;k<nbAsj;k++) {
		pSpotPrices[k] += deltaS;
		mSpotPrices[k] -= deltaS;
		simulatePaths();
		s = mPrice;
		setSpotPrices(mSpotPrices);
		simulatePaths();
		s1 = mPrice;
		setSpotPrices(pSpotPrices);
		simulatePaths();
		s2 = mPrice;
		
		delta += (s2-s1)/(2*deltaS);
		gamma += (s2 - 2*s+s1)/pow(deltaS,2);

		setSpotPrices(initialSpotPrices);
		pSpotPrices[k] -= deltaS;
		mSpotPrices[k] += deltaS;
	}
	delta /= nbAsj;
	gamma /= nbAsj;

	for(int k = 0;k<nbAsj;k++) {
		mCorrelations(k,k) += deltaSigma;
		pCorrelations(k,k) -= deltaSigma;

		setCorrelations(mCorrelations);
		if(simulationType == SimulationType::MonteCarlo || simulationType == SimulationType::VarAntithetique) {
			simulateRandVars();
		}
		else {
			simulatePseudoRandVars();
		}
		simulatePaths();
		s1 = mPrice;
		setCorrelations(pCorrelations);
		if(simulationType == SimulationType::MonteCarlo || simulationType == SimulationType::VarAntithetique) {
			simulateRandVars();
		}
		else {
			simulatePseudoRandVars();
		}
		simulatePaths();
		s2 = mPrice;
		vega += (s2-s1)/(2*deltaSigma);
		setCorrelations(initialCorrelations);
		if(simulationType == SimulationType::MonteCarlo || simulationType == SimulationType::VarAntithetique) {
			simulateRandVars();
		}
		else {
			simulatePseudoRandVars();
		}
		mCorrelations = initialCorrelations;
		pCorrelations = initialCorrelations;
	}
	
	vega  /=nbAsj;

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
}

void BestPlus::setPerformanceObj(double obj) {
	if(obj<=0) {
		throw out_of_range("L'objectif de performance doit être strictement positif");
	}
	else {
		performance_obj = obj;
	}
}
void BestPlus::setCapital(double c) {
	if(c<=0) {
		throw out_of_range("Le capital de départ doit être strictement positif");
	}
	else {
		capital = c;
	}
}


