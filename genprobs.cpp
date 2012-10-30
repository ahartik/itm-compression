#include <iostream>
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
		cout<<(long long)res[i][0]*TOTAL/total<<',';
//		cout<<res[i][0]<<' '<<res[i][1]<<'\n';
	}
	cout<<"};\n";
}
