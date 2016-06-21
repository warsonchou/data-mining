#include <iostream>
#include <fstream>  

using namespace std;
const int maxLength = 8000, maxCol = 390, rowNumber = 25010;
int featureSize = 384, groupNumber;
double eachGroup[rowNumber][maxCol], eachColMin[maxCol], eachColMax[maxCol],
thetaArr[385], afterThetaArr[385], alpha = 0.09, currentSum[rowNumber];

//归一化的预处理
void getMinAndMaxOfEachCol(string fileName) {
	char buf[maxLength];
	ifstream in;
	in.open(fileName, ios::in);

	int index = -1, numLen = 0, whichRow = 0;
	char tempStringNumber[50];
	while (in.getline(buf, maxLength)) {
		index = -1;
		numLen = 0;

		//第一行省去
		whichRow++;
		if (whichRow <= 1) {
			continue;
		}

		//获取一行当中384个数
		for (int i = 0; buf[i] != '\0'; i++) {
			//遇到一个,
			if (buf[i] == ',') {
				tempStringNumber[numLen] = '\0';
				if (index >= 0) {
					eachGroup[whichRow - 2][index] = atof(tempStringNumber);
				}
				numLen = 0;
				index++;
				continue;
			}

			tempStringNumber[numLen++] = buf[i];
		}
		eachGroup[whichRow - 2][featureSize] = atof(tempStringNumber);
	}
	groupNumber = whichRow - 1;
	alpha = alpha / (double)(groupNumber);
	in.close();
}

void regressionForEachParam(int whichParam) {
	int loop = 0;
	double allSum = 0;
	while (loop < groupNumber) {
		loop++;
		double xi = (whichParam == 0) ? 1 : eachGroup[loop][whichParam - 1];
		allSum += (currentSum[loop] - eachGroup[loop][featureSize]) * xi;
	}
	afterThetaArr[whichParam] = thetaArr[whichParam] - alpha * allSum; 
}

void training(string fileName) {
	//归一化前的处理
	getMinAndMaxOfEachCol(fileName);
	//使用alpha调节一千次
	int timesOfTraining = 5;
	while(timesOfTraining--) {
		//循环更新theta
		for(int i = 0; i < featureSize + 1; i++) {
			regressionForEachParam(i);
		}

		// double midSum = 0;
		// for (int i = 0; i < rowNumber; i++) {
		// 	midSum += (currentSum[i] - eachGroup[i][featureSize]) * (currentSum[i] - eachGroup[i][featureSize]);
		// }
		// cout << (midSum / groupNumber) << " ";

		for (int i = 0; i < featureSize; i++) {
			thetaArr[i] = afterThetaArr[i];
		}

		for (int i = 0; i < groupNumber; i++) {
			currentSum[i] = thetaArr[0];
			for (int j = 0; j < featureSize; j++) {
				currentSum[i] += thetaArr[j + 1] * eachGroup[i][j];
			}
		}
	}
	// cout << endl;
}

void getOutput(string testFileName) {
	ifstream test;
	test.open(testFileName, ios::in);
	ofstream ofile;
  ofile.open("output");
  ofile << "Id,reference\n";

  int index = -1, numLen = 0, whichRow = 0;
	char tempStringNumber[50], buf[maxLength];
	double tempRow[maxCol];

  while(test.getline(buf, maxLength)) {
  	index = -1;
		numLen = 0;
  	//第一行省去
		whichRow++;
		if (whichRow <= 1) {
			continue;
		}

		//获取一行当中384个数
		for (int i = 0; buf[i] != '\0'; i++) {
			//遇到一个,
			if (buf[i] == ',') {
				tempStringNumber[numLen] = '\0';
				if (index >= 0) {
					tempRow[index] = atof(tempStringNumber);
				}
				numLen = 0;
				index++;
				continue;
			}

			tempStringNumber[numLen++] = buf[i];
		}
		tempRow[index] = atof(tempStringNumber);

		double result = thetaArr[0];
		for (int i = 1; i <= featureSize; i++) {
			result += thetaArr[i] * tempRow[i - 1];
		}

		ofile << (whichRow - 2) << "," << result << "\n";
  }
  test.close();
  ofile.close();
}

int main(int argc, char const *argv[]) {

	string trainFileName, testFileName;
	cin >> trainFileName >> testFileName;
	//initial, start from 0
	// for (int i = 0; i < featureSize; i++) {
	// 	thetaArr[i] = 0;
	// 	afterThetaArr[i] = 0;
	// }
	//将结果读进theTa输出结果
	ifstream outputTrainingResult("trainingResult");
	char theTaStr[20], singleChar;
	int theTaStrLen = 0, theTaIndex = 0;
	while (outputTrainingResult.get(singleChar)) {
		if (singleChar == ' ') {
			theTaStr[theTaStrLen++] = '\0';
			theTaStrLen = 0;
			thetaArr[theTaIndex++] = atof(theTaStr);
			continue;
		}
		theTaStr[theTaStrLen++] = singleChar;
	}
	outputTrainingResult.close();

	groupNumber = 0;

	training(trainFileName);

	//将结果写入文件
	ofstream trainingResult("trainingResult");
	for(int i = 0; i < featureSize + 1; i++) {
		trainingResult << thetaArr[i] << " ";
	}
	trainingResult.close();

	// //将结果读进theTa输出结果
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

	getOutput(testFileName);

	return 0;
}