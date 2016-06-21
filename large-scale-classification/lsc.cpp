#include <iostream>
#include <fstream>  
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <future>
#include <utility>
#include <vector>

using namespace std;

 // rowNumber = 2177021 / 4 + 1
const int maxLength = 10000, maxCol = 125, rowNumber = 544811, featureSize = 3930;
size_t ptc = thread::hardware_concurrency() / 4 - 1;

unsigned int eachGroup[rowNumber][maxCol], resultOfClassify[rowNumber], eachGroupSize[rowNumber], 
	mask[4000], groupNumber, feature_number, timesOfTraining;

double thetaArr[featureSize + 1], afterThetaArr[featureSize + 1], alpha, 
	currentSum[rowNumber];

string trainFileName, testFileName, tResult;

//read data to eachGroup array
void readEachCol(string fileName) {
	char buf[maxLength];
	ifstream in;
	in.open(fileName.c_str(), ios::in);


	int index = 0, numLen = 0, whichRow = 0, n, quot;
	char tempStringNumber[10];
	while (in.getline(buf, maxLength)) {
		index = -1;
		numLen = 0;

		//获取一行当中3929个数
		for (int i = 0; buf[i] != '\0'; i++) {
			//遇到一个空格
			if (buf[i] == ' ') {
				tempStringNumber[numLen] = '\0';
				//分类的结果
				if (index < 0) {
					resultOfClassify[whichRow] = atoi(tempStringNumber);
				} else {
					n = atoi(tempStringNumber);
					quot = n / 32;
					eachGroup[whichRow][quot] = eachGroup[whichRow][quot] | mask[n];
				}
				index++;
				numLen = 0;
				continue;
			}
			tempStringNumber[numLen++] = buf[i];
		}
		n = atoi(tempStringNumber);
		quot = n / 32;
		eachGroup[whichRow][quot] = eachGroup[whichRow][quot] | mask[n];
		index++;
		eachGroupSize[whichRow] = index;
		whichRow++;
		// if (whichRow == 100000) {
		// 	break;
		// }
	}
	groupNumber = whichRow;
	// alpha = alpha / (double)(groupNumber);
	in.close();

	cout << "Load data ends!" << endl;
}

void getOutput(string testFileName) {
	ifstream test;
	test.open(testFileName.c_str(), ios::in);
	ofstream ofile;
  ofile.open("output.csv");
  ofile << "Id,label\n";

 	int numLen = 0, whichRow = 0, index, tempRow[4000];
	char tempStringNumber[100], buf[maxLength];

  while(test.getline(buf, maxLength)) {
		numLen = 0;
		index = -1;

		//获取一行当中3929个数
		for (int i = 0; buf[i] != '\0'; i++) {
			//遇到一个空格
			if (buf[i] == ' ') {
				tempStringNumber[numLen] = '\0';
				//取得特征值
				if (index >= 0) {
					tempRow[index] = atoi(tempStringNumber);
				}
				numLen = 0;
				index++;
				continue;
			}
			tempStringNumber[numLen++] = buf[i];
		}
		
		tempStringNumber[numLen] = '\0';
		tempRow[index++] = atoi(tempStringNumber);
		
		double result = 0;
		for (int i = 0; i < index; i++) {
			result += thetaArr[tempRow[i]];
		}
		if (result > pow((double)10, (double)-10)) {
			ofile << whichRow << "," << 1 << "\n";
		} else {
			ofile << whichRow << "," << 0 << "\n";
		}
		whichRow++;
  }
  cout << "Wirte data ends!" << endl;
  test.close();
  ofile.close();
}

void readBeforeTheta(string trainResult) {
	ifstream thetas;
	thetas.open(trainResult.c_str(), ios::in);
	int index = 0, strLen = 0;
	char tString[200], t;
	while(thetas.get(t)) {
		if (t == ' ') {
			tString[strLen++] = '\0';
			thetaArr[index++] = atof(tString);
			strLen = 0;
			continue;
		}
		tString[strLen++] = t;
	}
	thetaArr[index++] = atof(tString);
	afterThetaArr[index - 1] = thetaArr[index - 1]; 
	thetas.close();
}

double caculatePartSum(int start, int size, int whichParam) {
	double partSum = 0;
	int j = 0, startIndex, pos = whichParam / 32;
	for (int i = 0; i < size; i++) {
		startIndex = i + start;
		if (mask[whichParam] & eachGroup[startIndex][pos]) {
			partSum += currentSum[startIndex];
		}
	}
	return partSum;
}

void getPartCurrentSum(int start, int size) {
	int startIndex;
	unsigned int tempBit;

	for (int i = 0; i < size; i++) {
		startIndex = i + start;
		currentSum[startIndex] = 0;
		for (int j = 0; j < feature_number; j++) {
			if (j % 32 == 0) {
				tempBit = eachGroup[startIndex][j / 32];
			}
			if (mask[j] & tempBit) {
				currentSum[startIndex] += thetaArr[j];
			}
		}
		
		currentSum[startIndex] = (double)1 / ((double)1 + exp((double)0 - currentSum[startIndex])) - (double)resultOfClassify[startIndex];
	}
}


//using caculatePartSum
void regressionForEachParam(int whichParam) {
	int loop = 0, index = 0;
	double allSum = 0.0000;

	// multiple thread
	thread** tb = new thread*[ptc];
	vector<packaged_task<double(int, int, int)> > task_v;
	vector<future<double> > future_v;

 	for (int batch = 0; batch < ptc; batch++) {
 		size_t batch_each = groupNumber / ptc, start = batch * batch_each;
	 	if (batch == ptc - 1){
	 		batch_each = groupNumber - (groupNumber / ptc * batch);
	 	}
	 	packaged_task<double(int, int, int)> sub_pt(caculatePartSum);
	 	future_v.push_back(sub_pt.get_future());
	 	task_v.push_back(move(sub_pt));
	 	tb[batch] = new thread(move(task_v[batch]), start, batch_each, whichParam);
 	}

	for (int batch = 0; batch < ptc; batch++) {
		tb[batch]->join();
		allSum +=  future_v[batch].get();
		delete tb[batch];
	}
	delete tb;
	afterThetaArr[whichParam] = thetaArr[whichParam] - alpha * allSum;
}

//test
void getCost() {
	// test
	double cost = 0;
	for (int i = 0; i < groupNumber; i++) {
		if (resultOfClassify[i]) {
			cost -= log(currentSum[i] + (double)resultOfClassify[i]);
		} else {
			cost -= log((double)1 - currentSum[i]);
		}
	}
	cout << "Currrent cost: " << cost << endl;
}

void caculateCurrentSum() {
	// multiple thread
	thread** tb = new thread*[ptc];
	vector<packaged_task<void(int, int)> > task_v;

 	for (int batch = 0; batch < ptc; batch++) {
 		size_t batch_each = groupNumber / ptc, start = batch * batch_each;
	 	if (batch == ptc - 1){
	 		batch_each = groupNumber - (groupNumber / ptc * batch);
	 	}
	 	packaged_task<void(int, int)> sub_pt(getPartCurrentSum);
	 	task_v.push_back(move(sub_pt));
	 	tb[batch] = new thread(move(task_v[batch]), start, batch_each);
 	}

	for (int batch = 0; batch < ptc; batch++) {
		tb[batch]->join();
		delete tb[batch];
	}
	delete tb;
}


void training() {
	cout << "Training begins!" << endl;
	//使用alpha调节一千次	
	while(timesOfTraining--) {
		//循环更新theta
		for(int i = 0; i < feature_number; i++) {
			regressionForEachParam(i);
		}
		cout << timesOfTraining << " times training ends!" << endl;

		// update thetas
		int count = 0;
		for (int i = 0; i < feature_number; i++) {
			thetaArr[i] = afterThetaArr[i];
		}
		caculateCurrentSum();
		
		getCost();
	}
}

void initial() {
	// input 
	cin >> trainFileName >> testFileName >> tResult;

	groupNumber = 0;
	// include theta0
	feature_number = 3929;
	timesOfTraining = 150;
	alpha = 0.00000025;

	for (int i = 0; i < feature_number; i++) {
		thetaArr[i] = 0;
		afterThetaArr[i] = 0;
	}

	//produce mask for 4000 features, actually only 3929 features
	int lmove_step;
	for (int i = 0; i < 4000; i++) {
		mask[i] = 1;
		lmove_step = 31 - (i % 32);
		mask[i] = mask[i] << lmove_step;
	}

	//将数据读进内存
	// readEachCol(trainFileName);

	//initial currentSum
	// for (int i = 0; i < groupNumber; i++) {
	// 	currentSum[i] = 0.5 - (double)resultOfClassify[i];
	// }

	readBeforeTheta(tResult);
	// caculateCurrentSum();
	// getCost();

	cout << "Initial ends!" << endl;
	cout << "Currrent learning rate is: " << alpha << endl;
}

void writeResultToFile() {
	ofstream trainingResult(tResult.c_str());
	for(int i = 0; i < feature_number; i++) {
		if (i != 0) {
			trainingResult << " ";
		}
		trainingResult << thetaArr[i];
	}
	trainingResult << endl;
	trainingResult.close();
}

int main(int argc, char const *argv[]) {

	initial();
	// training();

	//将结果写入文件
	// writeResultToFile();

	// get the output
	getOutput(testFileName);
	return 0;
}