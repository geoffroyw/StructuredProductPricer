#ifndef ASSET_H
#define ASSET_H
#include <vector>


class Asset {
private:
	int m_nbTimeStep;
	double m_spotPrice;
	double m_volatitity;
	std::vector<double> m_path;

public:
	Asset(double spotPrice, double volatility);

	void setSpotPrice(double spotPrice);
	double getSpotPrice();
	void setVolatility(double volatility);
	double getVolatility();
	void simulatePath(double riskFreeRate, double maturity, double nbTimeStep);
	std::vector<double> getPath();
}


#endif