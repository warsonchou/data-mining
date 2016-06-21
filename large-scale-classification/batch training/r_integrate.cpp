#include <iostream>
#include <fstream>  
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <string.h>

using namespace std;

int featureSize;
double sub_thetaArr[4][4000];

void readBeforeTheta(string trainResult, int whichSub) {
	ifstream thetas;
	thetas.open(trainResult.c_str(), ios::in);
	int index = 0, strLen = 0;
	char tString[200], t;
	while(thetas.get(t)) {
		if (t == ' ') {
			tString[strLen++] = '\0';
			sub_thetaArr[whichSub][index++] = atof(tString);
			strLen = 0;
			continue;
		}
		tString[strLen++] = t;
	}
	sub_thetaArr[whichSub][index++] = atof(tString);

	if (featureSize < index) {
		featureSize = index;
	}

	thetas.close();
}

void getLastTheta(string r_integrate) {
	ofstream out;
	out.open(r_integrate.c_str());
	double l_r = 0;
	for (int i = 0; i < featureSize; i++) {
		double l_r = 0;
		for (int j = 0; j < 4; j++) {
			l_r += sub_thetaArr[j][i];
		}
		if (i) {
			out << " ";
		}
		out << l_r / (double)4;
	}
	out << endl;
	out.close();
}

int main(int argc, char const *argv[])
{
	string r[4], r_integrate;
	featureSize = 0;
	for (int i = 0; i < 4; i++) {
		cin >> r[i];
	}
	cin >> r_integrate;
	for (int i = 0; i < 4; i++) {
		readBeforeTheta(r[i], i);
	}
	getLastTheta(r_integrate);
	return 0;
}