#include <stdio.h>
#include <vector>
#include <iostream>
#include "BaseProduct.h"
#include "BestPlus.h"
#include "BonusCliquet.h"
#include "WinWin.h"
#include "SimulationType.h"
#include <boost/format.hpp>

using namespace std;


int main( int argc, char* argv[] )
{
	printf( "\nHello World\n\n" );

	BaseProduct *p = new BestPlus();
	/*(*p).setSimulationType(SimulationType::VarAntithetique);
	(*p).price();
	cout << "BESTPLUS\n";
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	//(*p).setDeltaS(10.0);
	(*p).computeGreeks();
	cout << "\n GREEKS \n";;
	cout << "Delta: " << boost::format("%1$.4f") % (*p).getDelta() << "\n";
	cout << "Gamma: " <<(*p).getGamma() << "\n";
	cout << "Theta: " <<(*p).getTheta() << "\n";
	cout << "Rho: " <<(*p).getRho() << "\n";
	cout << "Vega: " <<(*p).getVega() << "\n";
	cout << "\n\n\n";*/
	delete p;

	
	/*BonusCliquet *p = new BonusCliquet();
	(*p).setSimulationType(SimulationType::VarAntithetique);
	(*p).setSpotPrice(100.0);
	(*p).setMaturity(5.0);
	(*p).setVol(20.0/100.0);
	(*p).setRiskFreeRate(3.0/100.0);
	(*p).setHighBarrier(120.0);
	(*p).setLowBarrier(80.0);
	(*p).setNbSimulation(1000);
	(*p).setNbTimestep(52);

	(*p).price();
	(*p).computeGreeks();
	cout << "BONUSCLIQUET\n";
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	cout << "\n GREEKS \n";;
	cout << "Delta: " <<(*p).getDelta() << "\n";
	cout << "Gamma: " <<(*p).getGamma() << "\n";
	cout << "Theta: " <<(*p).getTheta() << "\n";
	cout << "Rho: " <<(*p).getRho() << "\n";
	cout << "Vega: " <<(*p).getVega() << "\n";
	cout << "\n\n\n";
	delete p;*/


	


	p = new WinWin();
	(*p).setSimulationType(SimulationType::MonteCarlo);
	(*p).price();
	cout << "WINWIN\n";
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	(*p).computeGreeks();
	cout << "\n GREEKS \n";;
	cout << "Delta: " <<(*p).getDelta() << "\n";
	cout << "Gamma: " <<(*p).getGamma() << "\n";
	cout << "Theta: " <<(*p).getTheta() << "\n";
	cout << "Rho: " <<(*p).getRho() << "\n";
	cout << "Vega: " <<(*p).getVega() << "\n";
	delete p;

	system("PAUSE");
}






