#include <iostream>
#include <fstream>  
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace std;

const int maxLength = 10000, maxCol = 125, rowNumber = 2177021, featureSize = 3930;
string input, output[4];
int lineNumber[4];

//read data to eachGroup array
void readDataIntoFour() {
	ifstream in;
	ofstream out[4];
	for (int i = 0; i < 4; i++) {	
		out[i].open(output[i].c_str());
	}
	in.open(input.c_str(), ios::in);

	char buf[maxLength];
	char tempStringNumber[10];
	time_t t;
	srand((unsigned)time(&t));
	while (in.getline(buf, maxLength)) {
		int whichFile = rand() % 4;
		lineNumber[whichFile]++;
		// string t = buf;
		out[whichFile] << buf << endl;
	}
	for (int i = 0; i < 4; i++) {
		out[i].close();
		//get the max linenumber
		if (lineNumber[0] < lineNumber[i])
			lineNumber[0] = lineNumber[i];
	}
	cout << lineNumber[0] << endl;
	in.close();
}

int main(int argc, char const *argv[])
{
	cin >> input;
	for (int i = 0; i < 4; i++) {
		cin >> output[i];
		lineNumber[i] = 0;
	}
	readDataIntoFour();
	return 0;
}