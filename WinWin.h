#ifndef WINWIN_H
#define WINWIN_H
#include "basketproduct.h"
class WinWin :
	public BasketProduct
{

private:
	double lowBarrier;
	double highBarrier;
	double capital;
	double finalCoupon;
	double barrierCoupon;
public:
	WinWin(void);
	~WinWin(void);
	void setHighBarrier(double b);
	void setLowBarrier(double b);
	virtual void price();
	virtual void computeGreeks();
	virtual void simulatePaths();
	virtual void simulateRandVars();
	void setCapital(double c);
	void setFinalCoupon(double c);
	void setBarrierCoupon(double c);
};

#endif