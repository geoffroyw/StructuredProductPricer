#ifndef BASEPRODUCT_H
#define BASEPRODUCT_H
class BaseProduct
{
protected:
	double mConfidenceLowerBound;
	double mConfidenceUpperBound;
	double mPrice;
public:
	BaseProduct(void);
	~BaseProduct(void);
	double getConfidenceLowerBound();
	double getConfidenceUpperBound();
	virtual void price();
	double getPrice();
};

#endif