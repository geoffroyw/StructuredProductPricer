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
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <stdexcept>

using namespace std;

WinWin::WinWin(void) : BasketProduct()
{
	setNbAsj(10);
	setMaturity(5.0);
	setNbTimestep(52*(int)maturity);
	setNbSimulation(1000);
	setBarrierCoupon(5.0);
	setFinalCoupon(10.0);
	setHighBarrier(0.35);
	setLowBarrier(0.01);
	setCapital(100);
	
	correlations.resize(nbAsj,nbAsj,false);
	cholM.resize(nbAsj,nbAsj,false);

	//On initialise tous les prix spots à 100
	for(int i =0;i<nbAsj;i++) {
		addSpotPrice(100.0);
	}

	// Initialisation de la matriec des corrélations
	for(unsigned i=0;i<correlations.size1();++i) {
		for(unsigned j=0;j<i;++j) {
			correlations(i,j) = 0.05;
		}
		correlations(i,i)=0.3;
	}
	
}


WinWin::~WinWin(void)
{
}


void WinWin::price() {
	simulateRandVars();
	simulatePaths();
	return;
}

void WinWin::simulateRandVars() {
	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
	double temp;
	
	cholesky();
	vector<double> randVars;

	for(int k = 0;k<nbSimulation;k++) {
		for(int l =0; l<nbTimestep; l++) {
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
			randVars.clear();
		}
	}
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

	bool highBarrierCross;
	bool lowBarrierCross;

	for(int k = 0; k<nbSimulation; k++) {
		highBarrierCross = false;
		lowBarrierCross = false;
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
		performance = 0.0;
		for(int j=1;j<nbTimestep;j++) {
			val_prec = 0.0;
			val = 0.0;
			for(int i = 0; i<nbAsj;i++) {
				val+=S_ts(i,j);
				val_prec+=S_ts(i,j-1);
			}
			performance = (val-val_prec)/val_prec;
			if((performance>=highBarrier && !highBarrierCross) || (performance<=lowBarrier && !lowBarrierCross)) {
				highBarrierCross = highBarrierCross ||  performance>=highBarrier;
				lowBarrierCross = lowBarrierCross || performance<=lowBarrier;
				TimestepsBarrierCross.push_back(j);
			}
		}

		payoff = 0.0;
		for(int i = 0; i<TimestepsBarrierCross.size(); i++) {
			payoff+=barrierCoupon*exp(-riskFreeRate*TimestepsBarrierCross[i]/52);
		}

		if(TimestepsBarrierCross.empty()) {
			payoff += finalCoupon*exp(-riskFreeRate*maturity);
		}
		payoff += capital*exp(-riskFreeRate*maturity);
		payoffs[k]=payoff;
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


/*(5.0);
	setFinalCoupon(10.0);
	setHighBarrier(0.35);
	setLowBarrier(0.01);
	setCapital(100);*/

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