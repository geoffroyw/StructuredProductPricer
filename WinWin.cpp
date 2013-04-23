/*
	Pricing d'un Win-Win 55
	Produit à capital garanti, composé d'un basket de 10 titres, avec deux barrière
	activantes : une à la hausse et une à la baisse suivant le rendement du
	portefeuille, versant dans ce cas là un coupon.
	Dans le cas où aucune barrière n'est franchie, le client touche un coupon final
*/

#include "WinWin.h"
#include "BasketProduct.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <stdexcept>
#include <boost/format.hpp>

using namespace std;

WinWin::WinWin(void) : BasketProduct()
{
	setNbAsj(10);
	setMaturity(5.0);
	setNbTimestep(2*(int)maturity);
	setNbSimulation(1000);
	setBarrierCoupon(5.0);
	setFinalCoupon(10.0);
	setHighBarrier(0.05);
	setLowBarrier(0.02);
	setCapital(100);
	
	correlations.resize(nbAsj,nbAsj,false);
	cholM.resize(nbAsj,nbAsj,false);

	//On initialise tous les prix spots à 100
	for(int i =0;i<nbAsj;i++) {
		addSpotPrice(100.0);
	}

	// Initialisation de la matriec des corrélations
	/*for(unsigned i=0;i<correlations.size1();++i) {
		for(unsigned j=0;j<i;++j) {
			correlations(i,j) = 0.05;
		}
		correlations(i,i)=0.3;
	}*/


	correlations(0,0) = 20/100.0;
	
	correlations(1,0) = 5/100.0;
	correlations(1,1) = 21/100.0;

	correlations(2,0) = 7/100.0;
	correlations(2,1) = 3/100.0;
	correlations(2,2) = 22/100.0;

	correlations(3,0) = 12/100.0;
	correlations(3,1) = 1/100.0;
	correlations(3,2) = 8/100.0;
	correlations(3,3) = 23/100.0;

	correlations(4,0) = 4/100.0;
	correlations(4,1) = 2/100.0;
	correlations(4,2) = 8/100.0;
	correlations(4,3) = 7/100.0;
	correlations(4,4) = 24/100.0;

	correlations(5,0) = 6/100.0;
	correlations(5,1) = 9/100.0;
	correlations(5,2) = 12/100.0;
	correlations(5,3) = 10/100.0;
	correlations(5,4) = 1/100.0;
	correlations(5,5) = 25/100.0;

	correlations(6,0) = 2/100.0;
	correlations(6,1) = 4/100.0;
	correlations(6,2) = 3/100.0;
	correlations(6,3) = 9/100.0;
	correlations(6,4) = 2/100.0;
	correlations(6,5) = 5/100.0;
	correlations(6,6) = 26/100.0;

	correlations(7,0) = 1/100.0;
	correlations(7,1) = 2/100.0;
	correlations(7,2) = 9/100.0;
	correlations(7,3) = 3/100.0;
	correlations(7,4) = 4/100.0;
	correlations(7,5) = 9/100.0;
	correlations(7,6) = 7/100.0;
	correlations(7,7) = 35/100.0;

	correlations(8,0) = 1/100.0;
	correlations(8,1) = 9/100.0;
	correlations(8,2) = 2/100.0;
	correlations(8,3) = 3/100.0;
	correlations(8,4) = 8/100.0;
	correlations(8,5) = 9/100.0;
	correlations(8,6) = 3/100.0;
	correlations(8,7) = 8/100.0;
	correlations(8,8) = 28/100.0;

	correlations(9,0) = 1/100.0;
	correlations(9,1) = 3/100.0;
	correlations(9,2) = 2/100.0;
	correlations(9,3) = 3/100.0;
	correlations(9,4) = 9/100.0;
	correlations(9,5) = 11/100.0;
	correlations(9,6) = 7/100.0;
	correlations(9,7) = 8/100.0;
	correlations(9,8) = 0/100.0;
	correlations(9,9) = 29/100.0;
	
}


WinWin::~WinWin(void)
{
}


void WinWin::price() {
	if(simulationType == SimulationType::MonteCarlo) {
		simulateRandVars();
	}
	else {
		simulatePseudoRandVars();
	}
	simulatePaths();
	return;
}

void WinWin::simulatePaths() {
	int randVarIndex = 0;
	double payoff = 0.0;
	double sum_payoffs = 0.0;
	double delta_t = maturity/nbTimestep;
	double r;
	double sd;
	double vol;
	boost::numeric::ublas::matrix<double> S_ts(nbAsj,nbTimestep);
	vector<double> payoffs(nbSimulation);

	double performance;
	double val_prec;
	double val;


	for(int k = 0; k<nbSimulation; k++) {
		vector<int> TimestepsBarrierCross;


		for(int i = 0; i<nbAsj;i++) {
			vol = correlations(i,i);
			r = (riskFreeRate-0.5*pow(vol,2))*delta_t;
			sd = vol*sqrt(delta_t);
			S_ts(i,0) = spotPrices[i];

			for(int j = 1; j<nbTimestep;j++) {
				S_ts(i,j)= S_ts(i,j-1)*exp(r+sd*mRandVars[randVarIndex++]);
			}
		}


		for(int j=1;j<nbTimestep;j++) {
			performance = 0.0;
			val_prec = 0.0;
			val = 0.0;
			for(int i = 0; i<nbAsj;i++) {
				val+=S_ts(i,j);
				val_prec+=S_ts(i,j-1);
			}

			val /= nbAsj;
			val_prec /=nbAsj;

			performance = (val-val_prec)/val_prec;
			if(performance>=highBarrier) {
				TimestepsBarrierCross.push_back(j);
			}
			if(performance<=lowBarrier) {
				TimestepsBarrierCross.push_back(j);
			}
		}

		payoff = 0.0;
		for(int i = 0; i<TimestepsBarrierCross.size(); i++) {
			payoff += barrierCoupon*exp(-riskFreeRate*TimestepsBarrierCross[i]/2);
		}

		if(TimestepsBarrierCross.empty()) {
			payoff += finalCoupon*exp(-riskFreeRate*maturity);
		}

		TimestepsBarrierCross.clear();
		payoff += capital*exp(-riskFreeRate*maturity);
		payoffs[k] = payoff;
		sum_payoffs += payoff;
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


void WinWin::computeGreeks(){
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
		pSpotPrices = initialSpotPrices;
		mSpotPrices = initialSpotPrices;
	}
	delta /= nbAsj;
	gamma /= nbAsj;

	for(int k = 0;k<nbAsj;k++) {
		mCorrelations(k,k) += deltaSigma;
		pCorrelations(k,k) -= deltaSigma;

		setCorrelations(mCorrelations);
		if(simulationType == SimulationType::MonteCarlo) {
			simulateRandVars();
		}
		else {
			simulatePseudoRandVars();
		}
		simulatePaths();
		s1 = mPrice;
		setCorrelations(pCorrelations);
		if(simulationType == SimulationType::MonteCarlo) {
			simulateRandVars();
		}
		else {
			simulatePseudoRandVars();
		}
		simulatePaths();
		s2 = mPrice;
		vega += (s2-s1)/(2*deltaSigma);
		setCorrelations(initialCorrelations);
		if(simulationType == SimulationType::MonteCarlo) {
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


void WinWin::setBarrierCoupon(double c) {
	if(c<=0) {
		throw out_of_range("Le coupon en cas de croisement d'une barrière doit être strictement positif");
	}
	else {
		barrierCoupon = c;
	}
}

void WinWin::setFinalCoupon(double c) {
	if(c<=0) {
		throw out_of_range("Le coupon final doit être strictement positif");
	}
	else {
		finalCoupon = c;
	}
}

void WinWin::setHighBarrier(double b) {
	if(b<=0) {
		throw out_of_range("La barrière haute doit être strictement positive");
	}
	else {
		highBarrier= b;
	}
}

void WinWin::setLowBarrier(double b) {
	if(b<=0) {
		throw out_of_range("La barrière basse doit être strictement positive");
	}
	else {
		lowBarrier = b;
	}
}

void WinWin::setCapital(double c) {
	if(c<=0) {
		throw out_of_range("Le capitaldoit être strictement positif");
	}
	else {
		capital = c;
	}
}