#include "BaseProduct.h"


BaseProduct::BaseProduct(void)
{
	mConfidenceLowerBound = 0.0;
	mConfidenceUpperBound = 0.0;
	mPrice = 0.0;
}


BaseProduct::~BaseProduct(void)
{
}

double BaseProduct::getConfidenceLowerBound() {
	return mConfidenceLowerBound;
}
double BaseProduct::getConfidenceUpperBound() {
	return mConfidenceUpperBound;
}
void BaseProduct::price(){}
double BaseProduct::getPrice() {
	return mPrice;
}

