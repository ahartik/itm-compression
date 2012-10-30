#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;
const int N = 100*1000;
int res[16][2];
const int TOTAL = 1<<30;
int main() {
	ifstream side("ex1_side.dat");
	ifstream data("ex1_class.dat");
	for(int i=0; i<N; ++i) {
		int m=0;
		for(int j=0; j<4; ++j) {
			int x;side>>x;
			m |= x<<j;
		}
		int v;data>>v;
		++res[m][v];
	}
	cout<<"const int totalprob = "<<TOTAL<<";\n";
	cout<<"const int cprobs[16] = {";
	for(int i=0; i<16; ++i) {
		int total = res[i][0] + res[i][1];
		cout << (long long)res[i][0]*TOTAL/total<<',';
//		cout<<res[i][0]<<' '<<res[i][1]<<'\n';
	}
	cout<<"};\n";
	cout<<"#ifdef _LATEX\n";
	cout<<"\\begin{tabular}{|c|c|c|c|c|c|}\n";
	cout<<"\\hline\n";
	cout<<"$x_1$ & $x_2$ & $x_3$ & $x_4$ & $P(x_0 = 0)$ & $P(x_0 = 1) $ \\\\ \\hline\n";
	for(int i=0; i<16; ++i) {
		double total = res[i][0] + res[i][1];
		for(int j=0;j<4;j++)
			cout<<((i>>j)&1)<<" & ";
		cout << res[i][0]/total<<" & ";
		cout << res[i][1]/total<<" \\\\ \\hline\n";
//		cout<<res[i][0]<<' '<<res[i][1]<<'\n';
	}
	cout<<"\\end{tabular}\n";

	double r=0;
	for(int i=0; i<16; ++i) {
		double a = res[i][0], b = res[i][1];
		double p = a/(a+b);
		r += (a+b) * (-p*log2(p) - (1-p)*log2(1-p));
	}
	cout<<r/8<<'\n';

	cout<<"#endif\n";
}
