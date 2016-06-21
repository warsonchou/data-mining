#include <iostream>
#include <stdio.h>

using namespace std;

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

int main(int argc, char const *argv[])
{
	int a[] = {1, 2, 3, 5, 6};
	int size = 5, target = 7;
	cout << binarySearch(a, size, target) << endl;
	return 0;
}