#include <iostream>
#include <fstream>  
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>

using namespace std;

#define MAX_EFFECTIVE_NUMBER 4000
const int feature_number = 11395, maxLength = 10000;
int effectiveFeatureIndex[MAX_EFFECTIVE_NUMBER], effectiveFeature, featureCount[feature_number],
	minExistingNumber = 10, maxItemNumber, hash_effective_feature_to_order[feature_number];


// just for auxiliary
void getEffectiveFeatureCount() {
  effectiveFeature = 0;
	for (int i = 0; i < feature_number + 1; i++) {
		if (featureCount[i]) {
			if (featureCount[i] > minExistingNumber) {
				effectiveFeatureIndex[effectiveFeature] = i;
				hash_effective_feature_to_order[i] = effectiveFeature;
				effectiveFeature++;
			}
		}
	}
	cout << effectiveFeature << endl;
}

void getFeatureCount(string fileName) {
	char buf[maxLength];
	ifstream in;
	in.open(fileName.c_str(), ios::in);

	// test
	memset(featureCount, 0, sizeof(featureCount));

	int index = 0, numLen = 0;
	char tempStringNumber[10];
	while (in.getline(buf, maxLength)) {
		index = 1;
		numLen = 0;

		// theta0 bias
		featureCount[0] += 1;

		//获取一行当中11392个数
		bool encounterColon = false;
		for (int i = 0; buf[i] != '\0'; i++) {
			//遇到一个空格
			if (buf[i] == ' ') {
				tempStringNumber[numLen] = '\0';
				//分类的结果
				if (encounterColon) {
					featureCount[atoi(tempStringNumber)] += 1;
				}
				numLen = 0;
				encounterColon = false;
				continue;
			}
			if (!encounterColon) {
				// :后面的数作废
				if (buf[i] == ':') {
					encounterColon = true;
					continue;
				}
				tempStringNumber[numLen++] = buf[i];
			}
		}
		featureCount[atoi(tempStringNumber)] += 1;
	}
	in.close();
}

void outMinfile(string inputFileName, string outfileName) {
	char buf[maxLength];
	ifstream in;
	in.open(inputFileName.c_str(), ios::in);
	ofstream ofile;
	ofile.open(outfileName.c_str());

	int numLen = 0, index;
	char tempStringNumber[10];
	while (in.getline(buf, maxLength)) {
		numLen = 0;
		index = 1;

		//获取一行当中11392个数
		bool encounterColon = false;
		for (int i = 0; buf[i] != '\0'; i++) {
			//遇到一个空格
			if (buf[i] == ' ') {
				tempStringNumber[numLen] = '\0';
				//分类的结果
				if (!encounterColon) {
					//output result and theta0
					ofile << tempStringNumber << " 0";
				} else if (featureCount[atoi(tempStringNumber)] > minExistingNumber) {
					index++;
					ofile << " " << hash_effective_feature_to_order[atoi(tempStringNumber)];
				}
				numLen = 0;
				encounterColon = false;
				continue;
			}
			if (!encounterColon) {
				// :后面的数作废
				if (buf[i] == ':') {
					encounterColon = true;
					continue;
				}
				tempStringNumber[numLen++] = buf[i];
			}
		}
		if (featureCount[atoi(tempStringNumber)] > minExistingNumber) {
			ofile << " " << hash_effective_feature_to_order[atoi(tempStringNumber)] << "\n";
			index++;
		} else {
			ofile << "\n";
		}
		maxItemNumber = maxItemNumber > index ? maxItemNumber : index;
	}
	cout << maxItemNumber << endl;
	in.close();
	ofile.close();
}

int main(int argc, char const *argv[])
{
	string trainfile, outTrainFile, testfile, outTestfile;
	cin >> trainfile >> outTrainFile >> testfile >> outTestfile;
	// get info of each line
	getFeatureCount(trainfile);
	getEffectiveFeatureCount();
	// outMinfile(trainfile, outTrainFile);
	outMinfile(testfile, outTestfile);
	return 0;
}