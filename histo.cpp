#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
using namespace std;
const int S = 512;
int mat[S][S];
int BS = 16;
const int M = 1<<14;
int arr[15][2*M];
double vars[15];
int main() {
	ifstream in("kiel.arr");
	for(int i=0; i<S; ++i) for(int j=0; j<S; ++j) in>>mat[i][j];

	for(int i=0; i<5; ++i) {
		for(int j=1; j<4; ++j) {
			int s = 16<<i;
			int sy = s*(j&1), sx = s*(j>>1);
			int block = 3*i+(j-1);
			double ssum=0;
			for(int y=0; y<s; ++y) for(int x=0; x<s; ++x) {
				int v = mat[sy+y][sx+x];
				arr[block][M + v]++;
				ssum += v*v;
			}
			vars[block] = ssum / (s*s);
		}
	}
	cout<<"static const float gdevs[15] = {";
	for(int i=0; i<15; ++i) cout<<sqrt(vars[i])<<',';
	cout<<"};\n";

	for(int k=-2000; k<2000; ++k) {
		for(int i=0; i<15; ++i) {
			cout<<(","+!i)<<arr[i][M+k];
		}
		cout<<'\n';
	}
}
