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
	void price();
	void setSpotPrice(double price);
	void setLowBarrier(double barrier);
	void setHighBarrier(double barrier);
	void setVol(double v);

};



#endif