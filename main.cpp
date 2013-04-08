#include <stdio.h>
#include <vector>
#include <iostream>
#include "BaseProduct.h"
#include "BestPlus.h"
#include "BonusCliquet.h"
#include "WinWin.h"


using namespace std;


int main( int argc, char* argv[] )
{
	/*QApplication app(argc,argv);
	Ui::MainWindow ui;
	QMainWindow *mainWindow = new QMainWindow();
	ui.setupUi(mainWindow);
	mainWindow->show();
	

	return app.exec();*/




	printf( "\nHello World\n\n" );
	
	BaseProduct *p = new BestPlus();
	(*p).price();
	cout << "BESTPLUS\n";
	cout << (*p).getPrice() <<"\n";
	cout << "Inf : " << (*p).getConfidenceLowerBound() << "\n";
	cout << "Sup : " << (*p).getConfidenceUpperBound() << "\n";
	(*p).setDeltaS(10);
	(*p).computeGreeks();
	cout << "\n GREEKS \n";;
	cout << "Delta: " <<(*p).getDelta() << "\n";
	cout << "Gamma: " <<(*p).getGamma() << "\n";
	cout << "Theta: " <<(*p).getTheta() << "\n";
	cout << "Rho: " <<(*p).getRho() << "\n";
	cout << "Vega: " <<(*p).getVega() << "\n";*/
	cout << "\n\n\n";
	delete p;

	
	p = new BonusCliquet();
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
	delete p;


	p = new WinWin();
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






