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
const int maxLength = 10000, maxCol = 125, rowNumber = 2177021, featureSize = 11392;
size_t ptc = thread::hardware_concurrency();

unsigned int eachGroup[rowNumber][maxCol], resultOfClassify[rowNumber], eachGroupSize[rowNumber], mask[4000];
// 总共有个参数featureSize + 1
int groupNumber, effectiveFeature = 0, effectiveFeatureIndex[4000];
double thetaArr[featureSize + 1], afterThetaArr[featureSize + 1], alpha = 0.00001, currentSum[rowNumber];
long featureCount[featureSize + 1];
string trainFileName, testFileName;

// count the item number of one line
// int itemNumberPerLine = 0; 

void initial() {
	//produce mask for 4000 features
	int lmove_step;
	for (int i = 0; i < 4000; i++) {
		mask[i] = 1;
		lmove_step = 31 - (i % 32);
		mask[i] = mask[i] << lmove_step;
	}
}


// just for auxiliary
void outputFeatureCount() {
	ofstream ofile;
  ofile.open("featureCount");
  effectiveFeature = 0;
	for (int i = 0; i < featureSize + 1; i++) {
		if (featureCount[i]) {
			ofile << i << ":" << featureCount[i] << " ";
			if (featureCount[i] > 10) {
				effectiveFeatureIndex[effectiveFeature] = i;
				effectiveFeature++;
			}
		}
	}
	ofile << endl << endl;

	sort(featureCount, featureCount + featureSize + 1);
	for (int i = 0; i < featureSize + 1; i++) {
		ofile << featureCount[i] << " ";
	}
	ofile << endl << endl;
	ofile << "It has " << effectiveFeature << " effective feature!" << endl;
	ofile.close();
}

void readEachCol(string fileName) {
	char buf[maxLength];
	ifstream in;
	in.open(fileName.c_str(), ios::in);

	// test
	memset(featureCount, 0, sizeof(featureCount));

	int index = 0, numLen = 0, whichRow = 0, n;
	char tempStringNumber[10];
	while (in.getline(buf, maxLength)) {
		index = 1;
		numLen = 0;

		eachGroup[whichRow][0] = eachGroup[whichRow][0] | mask[0];
		featureCount[0] += 1;

		//获取一行当中11392个数
		bool encounterColon = false;
		for (int i = 0; buf[i] != '\0'; i++) {
			//遇到一个空格
			if (buf[i] == ' ') {
				tempStringNumber[numLen] = '\0';
				//分类的结果
				if (!encounterColon) {
					resultOfClassify[whichRow] = atoi(tempStringNumber);
				} else {
					n = atoi(tempStringNumber), quot = n / 32;
					eachGroup[whichRow][quot] = eachGroup[whichRow][quot] | mask[n];
					index++;
					featureCount[n] += 1;
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
		n = atoi(tempStringNumber);
		eachGroup[whichRow][n / 32] = eachGroup[whichRow][n / 32] | mask[n];
		index++;
		featureCount[n] += 1;
		eachGroupSize[whichRow] = index;
		whichRow++;
	}
	groupNumber = whichRow;
	// alpha = alpha / (double)(groupNumber);
	in.close();
	outputFeatureCount();
}

void getOutput(string testFileName) {
	ifstream test;
	test.open(testFileName.c_str(), ios::in);
	// ofstream ofile;
 //  ofile.open("output.csv");
 //  ofile << "Id,label\n";

  int index = 0, numLen = 0, whichRow = 0;
	char tempStringNumber[10], buf[maxLength];
	int tempRow[maxCol];

	// for test
	int correctCount = 0, actualResult;

  // while(test.getline(buf, maxLength)) {
  // 	index = 1;
		// numLen = 0;

		// tempRow[0] = 1;
		// //获取一行当中11392个数
		// bool encounterColon = false;
		// for (int i = 0; buf[i] != '\0'; i++) {
		// 	//遇到一个空格
		// 	if (buf[i] == ' ') {
		// 		tempStringNumber[numLen] = '\0';
		// 		//取得特征值
		// 		if (encounterColon) {
		// 			tempRow[index++] = atoi(tempStringNumber);
		// 		} else {
		// 			actualResult = atoi(tempStringNumber);
		// 		}
		// 		numLen = 0;
		// 		encounterColon = false;
		// 		continue;
		// 	}
		// 	if (!encounterColon) {
		// 		// :后面的数作废
		// 		if (buf[i] == ':') {
		// 			encounterColon = true;
		// 			continue;
		// 		}
		// 		tempStringNumber[numLen++] = buf[i];
		// 	}
		// }
		// tempStringNumber[numLen] = '\0';
		// tempRow[index++] = atoi(tempStringNumber);

		// double result = 0;
		// for (int i = 0; i < index; i++) {
		// 	if (thetaArr[tempRow[i]]) {
		// 		result += thetaArr[tempRow[i]];
		// 	}
		// }
		// result = (double)1 / ((double)1 + exp((double)0 - result));

		// int lastClassification = (fabs(result - 0.5) < pow((double)10, (double)-8)) ? 0 : 1;
		// // ofile << whichRow << "," << lastClassification << "\n";

		// whichRow++;

		// // test 
		// if (lastClassification == actualResult) {
		// 	correctCount++;
		// }

  // }
  // test.close();
  // ofile.close();

  // test
  for (int i = 0; i < rowNumber; i++) {
  	double result = 0;
  	unsigned int tempBit, currentBit;
		for (int j = 0; j < effectiveFeature + 1; j++) {
			if (j / 32 == 0) {
				tempBit = eachGroup[i][j / 32];
			}
			if (mask[j] & tempBit) {
				if (thetaArr[j]) {
					result += thetaArr[eachGroup[i][j]];
				}
			}
			
		}
		result = (double)1 / ((double)1 + exp((double)0 - result));

		int lastClassification = (fabs(result - 0.5) < pow((double)10, (double)-8)) ? 0 : 1;

		// test 
		if (lastClassification == resultOfClassify[i]) {
			correctCount++;
		}
  }

  //test
  cout << (double)correctCount / (double)rowNumber * (double)10 << "%" << endl;
}
// int compareints(const void *a, const void *b) {
// 	return (*(int*)a - *(int*)b);
// }

int binarySearch(int* arr, int &size, int &val) {
	int low = 0, high = size;
	while(low <= high) {
		int mid = (low + high) / 2;
		if (arr[mid] > val) {
			high = mid - 1;
		} else if (arr[mid] < val) {
			low = mid + 1;
		} else  {
			return 1;
		}
	}
	return 0;
}

double caculatePartSum(int start, int size, int whichParam) {
	double partSum = 0;
	int j = 0;
	for (int i = 0; i < size; i++) {
		// if (binarySearch(eachGroup[i + start], eachGroupSize[i + start], whichParam)) {
		// 	partSum += currentSum[i + start];
		// }
	}
	return partSum;
}

void regressionForEachParam(int whichParam) {
	int loop = 0, index = 0;
	double allSum = 0.0000;
	// while (loop < groupNumber) {
	// 	// if ((int*)bsearch(&whichParam, eachGroup[loop], eachGroupSize[loop], sizeof(int), compareints) != NULL) {
	// 	// if (find(eachGroup[loop] + 0, eachGroup[loop] + eachGroupSize[loop], whichParam) != eachGroup[loop] + eachGroupSize[loop]) {
	// 	if (binarySearch(eachGroup[loop], eachGroupSize[loop], whichParam)) {
	// 		allSum += currentSum[loop];
	// 	}
	// 	loop++;
	// }

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

void training(string fileName) {
	//将数据读进内存
	readEachCol(fileName);
	
	cout << "Training begins!" << endl;
	//使用alpha调节一千次	
	int timesOfTraining = 3;
	while(timesOfTraining--) {

		//循环更新theta
		for(int i = 0; i < effectiveFeature; i++) {
			// cout << timesOfTraining << " times of " << effectiveFeatureIndex[i] << " parameters... " << endl;
			regressionForEachParam(effectiveFeatureIndex[i]);
		}
		cout << timesOfTraining << " times training ends!" << endl;

		// effectiveFeature 
		for (int i = 0; i < effectiveFeature; i++) {
			thetaArr[effectiveFeatureIndex[i]] = afterThetaArr[effectiveFeatureIndex[i]];
		}

		//test
		// int correctCount = 0;

		for (int i = 0; i < groupNumber; i++) {
			for (int j = 0; j < eachGroupSize[i]; j++) {
				if (thetaArr[eachGroup[i][j]]) {
					currentSum[i] += thetaArr[eachGroup[i][j]];
				}
			}
			currentSum[i] = (double)1 / ((double)1 + exp((double)0 - currentSum[i])) - (double)resultOfClassify[i];
		
			// test
			// double result = currentSum[i] + (double)resultOfClassify[i];
			// int lastClassification = (fabs(result - 0.5) < pow((double)10, (double)-8)) ? 0 : 1;
			// if (lastClassification == resultOfClassify[i]) {
			// 	correctCount++;
			// }
		}

		//test
		getOutput(testFileName);
	}
}


int main(int argc, char const *argv[]) {

	cin >> trainFileName >> testFileName;
	// initial, start from 0
	for (int i = 0; i < featureSize + 1; i++) {
		thetaArr[i] = 0;
		afterThetaArr[i] = 0;
	}

	//将结果读进theTa
	// ifstream outputTrainingResult("trainingResult");
	// char theTaStr[20], singleChar;
	// int theTaStrLen = 0, theTaIndex = 0;
	// bool encounterColon = false;
	// effectiveFeature = 0;
	// while (outputTrainingResult.get(singleChar)) {
	// 	//遇到一个空格
	// 	if (singleChar == ' ') {
	// 		theTaStr[theTaStrLen] = '\0';
	// 		thetaArr[theTaIndex] = atof(theTaStr);
	// 		theTaStrLen = 0;
	// 	} else if (singleChar == ':') {
	// 		theTaStr[theTaStrLen] = '\0';
	// 		theTaIndex = atoi(theTaStr);
	// 		theTaStrLen = 0;
	// 	} else {
	// 		theTaStr[theTaStrLen++] = singleChar;
	// 	}
	// }
	// theTaStr[theTaStrLen] = '\0';
	// thetaArr[theTaIndex] = atof(theTaStr);
	// outputTrainingResult.close();

	groupNumber = 0;
	initial();
	training(trainFileName);

	//将结果写入文件
	ofstream trainingResult("trainingResult");
	for(int i = 0; i < effectiveFeature; i++) {
		trainingResult << effectiveFeatureIndex[i] << ":" << thetaArr[effectiveFeatureIndex[i]] << " ";
	}
	trainingResult << endl;
	trainingResult.close();

	//将结果读进theTa, 输出结果
	// ifstream outputTrainingResult("trainingResult");
	// char theTaStr[20], singleChar;
	// int theTaStrLen = 0, theTaIndex = 0;
	// while (outputTrainingResult.get(singleChar)) {
	// 	if (singleChar == ' ') {
	// 		theTaStr[theTaStrLen++] = '\0';
	// 		theTaStrLen = 0;
	// 		thetaArr[theTaIndex++] = atof(theTaStr);
	// 		continue;
	// 	}
	// 	theTaStr[theTaStrLen++] = singleChar;
	// }
	// outputTrainingResult.close();

	// getOutput(testFileName);

	return 0;
}