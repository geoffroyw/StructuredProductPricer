#ifndef BONUSCLIQUET_H
#define BONUSCLIQUET_H
#include "BaseProduct.h"

class BonusCliquet : public BaseProduct 
{
private:
	double spotPrice;
	double lowBarrier;
	double highBarrier;
	double vol;

public:
	BonusCliquet(void);
	~BonusCliquet(void);
	virtual void price();
	virtual void computeGreeks();
	virtual void simulatePaths();
	virtual void simulateRandVars();
	void setSpotPrice(double price);
	void setLowBarrier(double barrier);
	void setHighBarrier(double barrier);
	void setVol(double v);
	void setMaturity(double m);

};



#endif