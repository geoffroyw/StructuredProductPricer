#include <stdio.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include "BestPlus.h"
#include "BonusCliquet.h"


using namespace std;


int main( int argc, const char* argv[] )
{
	printf( "\nHello World\n\n" );
	
	BestPlusPrice();
	cout << "\n\n\n";
	BonusCliquetPrice();

	system("PAUSE");
}






