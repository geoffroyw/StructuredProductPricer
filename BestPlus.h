#ifndef BESTPLUS_H
#define BESTPLUS_H
#include "BasketProduct.h"
class BestPlus : public BasketProduct 
{
private:
	double performance_obj;
	double capital;
public:
	BestPlus(void);
	~BestPlus(void);
	virtual void price();
	virtual void computeGreeks();
	virtual void simulatePaths();
	virtual void simulateRandVars();
	void setPerformanceObj(double obj);
	void setCapital(double c);
	

};



#endif