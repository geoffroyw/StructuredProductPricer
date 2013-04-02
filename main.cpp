#include <stdio.h>
#include <iostream>
#include "BestPlus.h"
#include "BonusCliquet.h"


using namespace std;


int main( int argc, const char* argv[] )
{
	printf( "\nHello World\n\n" );
	
	BaseProduct *p = new BestPlus();
	(*p).price();
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	delete p;

	
	p = new BonusCliquet();
	(*p).price();
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	delete p;

	system("PAUSE");
}






