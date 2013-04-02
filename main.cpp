#include <stdio.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

using namespace std;

void BestPlusPrice();
void BonusCliquetPrice();

int main( int argc, const char* argv[] )
{
	printf( "\nHello World\n\n" );
	
	BestPlusPrice();
	cout << "\n\n\n";
	BonusCliquetPrice();

	system("PAUSE");
}



void BestPlusPrice() {
	int nb_ASJ = 14;
	double maturity = 7.0;
	double performance_obj = 0.35;
	int nbTimeStep = 7;
	int nbSimulation = 1000;
	double sum_payoffs(0);
	double riskFreeRate = 0.03;
	double capital = 100.0;


	vector<double> spot_prices(nb_ASJ);
	vector<double> saved_pf_value(nbTimeStep);
	vector<double> payoffs(nbSimulation);
	boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::lower> correlations(nb_ASJ,nb_ASJ);
	boost::numeric::ublas::matrix<double> cholM(nb_ASJ,nb_ASJ);

	saved_pf_value[0]=0;


	//On initialise tous les prix spots à 100
	for(int i =0;i<nb_ASJ;i++) {
		spot_prices.push_back(100.0);
		saved_pf_value[0]+=100;
	}

	saved_pf_value[0]/=nb_ASJ;
	for(unsigned i=0;i<correlations.size1();++i) {
		for(unsigned j=0;j<i;++j) {
			correlations(i,j) = 0.05+i/(j+1);
		}
		correlations(i,i)=0.3;
	}

	//Décomposition de Cholesky de la matrice des corrélations
	double x;
	for(unsigned i=0;i<cholM.size1();++i) {
		for(unsigned j=0;j<cholM.size2();++j) {
			cholM(i,j)=0;
		}
	}
	for(int i=0;i<cholM.size1();++i) {
		for(int j=i;j<cholM.size2();++j) {
			x = correlations(i,j);
			for(int k=0;k<i-1;++k) {
				x = x-cholM(i,k)*cholM(j,k);
			}
			if(i==j) {
				cholM(i,j)=sqrt(x);
			}
			else {
				cholM(j,i)=x/cholM(i,i);
			}
		}
	}
	


	//Simulation
	double delta_t = maturity/nbTimeStep;
	double r;
	double sd;
	vector<double> vols;
	double vol;
	double temp;
	boost::numeric::ublas::matrix<double> S_ts(nb_ASJ,nbTimeStep);
	vector<double> randVars(nb_ASJ);
	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
	double best_pf_performance=0;
	int timestep_first_coupon = -1;
	double payoff = 0;

	for(int k = 0;k<nbSimulation;k++) {
		//On génère des vars indépendantes distribuées normalement
		for(int i = 0; i<nb_ASJ; i++) {
			randVars.push_back(var_nor());
		}
		
		for(int i=0;i<nb_ASJ;i++) {
			temp=0;
			for(int j=0;j<=i;j++) {
				temp+=randVars[j]*cholM(i,j);
			}
			vols.push_back(temp);
		}
		
		
		//Simulation des trajectoires
		for(int i = 0; i<nb_ASJ;i++) {
			vol = correlations(i,i);
			r = (riskFreeRate-0.5*pow(vol,2))*delta_t;
			sd = vol*sqrt(delta_t);
			S_ts(i,0) = spot_prices[i];

			for(int j = 1; j<nbTimeStep;j++) {
				S_ts(i,j)= S_ts(i,j-1)*exp(r+sd*vols[i]);
			}
		}

		boost::numeric::ublas::matrix<double> performance(nb_ASJ,nbTimeStep);
		vector<double> saved_performance_index(nb_ASJ);
		
		int best_perf1_index(1), best_perf2_index(0);
		double best_perf1_value(1), best_perf2_value(0);
		//A chaque date anniversaire, on prend les deux meilleures performances


		for(int j=1;j<nbTimeStep;j++) {

			for(int i = 0;i<nb_ASJ;i++) {
				if(std::find(saved_performance_index.begin(), saved_performance_index.end(), i) == saved_performance_index.end()) {

					performance(i,j)=(S_ts(i,j)-S_ts(i,j-1))/S_ts(i,j-1);
					if(performance(i,j)>=best_perf1_value) {
						best_perf2_value = best_perf1_value;
						best_perf2_index = best_perf1_index;
						best_perf1_value = performance(i,j);
						best_perf1_index = i; 
					}
					if(performance(i,j)>best_perf2_value) {
						best_perf2_value = performance(i,j);
						best_perf2_index = i; 
					}
				}
			}
			saved_performance_index.push_back(best_perf1_value);
			saved_performance_index.push_back(best_perf2_value);
			for(int l=j+1;l<nbTimeStep-1;l++) {
				performance(best_perf1_index,l)=performance(best_perf1_index,j);
				performance(best_perf2_index,l)=performance(best_perf2_index,j);
			}

			//Calcul de la valeur du PF sécurisée
			saved_pf_value[j]=0;
			for(int i=0;i<nb_ASJ;i++) {
				saved_pf_value[j]+=S_ts(i,j);
			}
			saved_pf_value[j]/=nb_ASJ;

			if(best_pf_performance<(saved_pf_value[j]-saved_pf_value[j-1])/saved_pf_value[j-1]) {
				best_pf_performance = (saved_pf_value[j]-saved_pf_value[j-1])/saved_pf_value[j-1];
			}

			//Est-ce qu'on donne un coupon ?
			if(best_pf_performance>=performance_obj && timestep_first_coupon<0) {
				timestep_first_coupon = j;
			}

		}

		// Payoff = NPV des cash-flows
		double memory_coupon =0;
		payoff = 0;
		if(timestep_first_coupon>=0) {
			memory_coupon = timestep_first_coupon*0.07;
			payoff = capital*memory_coupon*exp(-riskFreeRate*timestep_first_coupon);

			for(int i = timestep_first_coupon; i<nbTimeStep;i++) {
				payoff += capital*0.07*exp(-riskFreeRate*i);
			}
		}
		payoff+=capital*(1+best_pf_performance)*exp(-riskFreeRate*(nbTimeStep));
		payoffs.push_back(payoff);

		sum_payoffs+=payoff;
	}

	double prix = sum_payoffs/nbSimulation; 
	cout << "Prix : ";
	cout << prix;
	cout << "€\n";

	//Variance des payoffs
	double var = 0;
	for(int i=0;i<nbSimulation;i++){
		var+=pow((payoffs[i]-sum_payoffs/nbSimulation),2);
	}
	var/=(nbSimulation-1);
	var=sqrt(var);

	cout << "Intervalle de confiance à 95%\n";
	cout << "Borne inf : " << prix-2*var/sqrt(nbSimulation) << "\n";
	cout << "Borne sup : " << prix+2*var/sqrt(nbSimulation) << "\n";
}


void BonusCliquetPrice() {
	double maturity = 7.0;
	double performance_obj = 0.35;
	int nbTimeStep = 52*maturity;
	int nbSimulation = 1000;
	double sum_payoffs(0);
	double riskFreeRate = 0.03;
	double spotPrice = 100.0;
	double low_barrier = 80;
	double high_barrier = 120;
	double vol = 0.3;
	vector<double> payoffs;
	double payoff;


	bool low_barrier_reach = false;
	bool high_barrier_reach = true;

	boost::mt19937 rng; 
	boost::normal_distribution<> nd(0.0, 1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);


	double delta_t = maturity/nbTimeStep;
	double r;
	double sd;
	double S_t;

	for(int k=0;k<nbSimulation;k++) {
		high_barrier_reach = false;
		low_barrier_reach = false;


		//On simule les trajectoires
		r = (riskFreeRate-0.5*pow(vol,2))*delta_t;
		sd = vol*sqrt(delta_t);
		S_t = spotPrice;
		for(int j = 1; j<nbTimeStep;j++) {
			S_t= S_t*exp(r+sd*var_nor());

			if(S_t>=high_barrier) {
				high_barrier_reach = true;
			}
			if(S_t<=low_barrier) {
				low_barrier_reach = true;
			}
		}

		payoff = 0;
		if(!low_barrier_reach) {
			if(S_t>high_barrier) {
				payoff=S_t;
			}
			else {
				payoff=high_barrier;
			}
		}
		else {
			if(high_barrier_reach) {
				payoff = high_barrier;
			}
			else {
				payoff=S_t;
			}
		}

		payoff*=exp(-riskFreeRate*maturity);
		sum_payoffs+=payoff;
		payoffs.push_back(payoff);
	}

	double prix = sum_payoffs/nbSimulation; 
	cout << "Prix : ";
	cout << prix;
	cout << "€\n";

	//Variance des payoffs
	double var = 0;
	for(int i=0;i<nbSimulation;i++){
		var+=pow((payoffs[i]-sum_payoffs/nbSimulation),2);
	}
	var/=(nbSimulation-1);
	var=sqrt(var);

	cout << "Intervalle de confiance à 95%\n";
	cout << "Borne inf : " << prix-2*var/sqrt(nbSimulation) << "\n";
	cout << "Borne sup : " << prix+2*var/sqrt(nbSimulation) << "\n";

}