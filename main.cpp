#include <stdio.h>
#include <vector>
#include <iostream>
#include "BaseProduct.h"
#include "BestPlus.h"
#include "BonusCliquet.h"
#include "WinWin.h"

using namespace std;


int main( int argc, const char* argv[] )
{
	printf( "\nHello World\n\n" );
	
	BaseProduct *p = new BestPlus();
	/*(*p).price();
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	(*p).setDeltaS(10);*/
	/*(*p).computeGreeks();
	cout << "\n GREEKS \n";;
	cout << "Delta: " <<(*p).getDelta() << "\n";
	cout << "Gamme: " <<(*p).getGamma() << "\n";
	cout << "Theta: " <<(*p).getTheta() << "\n";
	cout << "Rho: " <<(*p).getRho() << "\n";
	cout << "Vega: " <<(*p).getVega() << "\n";*/
	//cout << "\n\n\n";
	delete p;

	
	p = new BonusCliquet();
	(*p).price();
	(*p).computeGreeks();
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	cout << "\n GREEKS \n";;
	cout << "Delta: " <<(*p).getDelta() << "\n";
	cout << "Gamme: " <<(*p).getGamma() << "\n";
	cout << "Theta: " <<(*p).getTheta() << "\n";
	cout << "Rho: " <<(*p).getRho() << "\n";
	cout << "Vega: " <<(*p).getVega() << "\n";
	cout << "\n\n\n";
	delete p;


	p = new WinWin();
	(*p).price();
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	(*p).computeGreeks();
	cout << "\n GREEKS \n";;
	cout << "Delta: " <<(*p).getDelta() << "\n";
	cout << "Gamme: " <<(*p).getGamma() << "\n";
	cout << "Theta: " <<(*p).getTheta() << "\n";
	cout << "Rho: " <<(*p).getRho() << "\n";
	cout << "Vega: " <<(*p).getVega() << "\n";
	delete p;

	system("PAUSE");
}






