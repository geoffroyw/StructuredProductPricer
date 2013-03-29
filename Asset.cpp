/**
	Asset Class
**/

#include <vector>
#include "Asset.h";
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

using namespace std;

/**
	Constructeur
	Par défaut, on fixe le nombre de pas de temps à 52.
**/
Asset::Asset(double spotPrice, double volatility) {
	setSpotPrice(spotPrice);
	setVolatility(volatility);
}

void Asset::setSpotPrice(double spotPrice) {
	if(spotPrice <=0) {
		throw domain_error("Prix négatif");
	}
	m_spotPrice = spotPrice;
}

double Asset::getSpotPrice() {
	return m_spotPrice;
}



void Asset::setVolatility(double vol) {
	m_volatitity = vol;
}

double Asset::getVolatility() {
	return m_volatitity;
}

/**
	Simulate log_normal underlying asset path.
**/
void Asset::simulatePath(double riskFreeRate, double maturity, double nbTimeStep) {
	double delta_t = maturity/nbTimeStep;
	double r = (riskFreeRate-0.5*pow(m_volatitity,2))*delta_t;
	double sd = m_volatitity*sqrt(delta_t);
	double S_t = m_spotPrice;

	boost::mt19937 rng;
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);

	for(int i = 0; i<m_nbTimeStep;i++) {
		S_t = S_t*exp(r+sd*var_nor());
		m_path[i]=S_t;
	}
}

vector<double> Asset::getPath() {
	return m_path;
}