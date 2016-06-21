#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <future>
using namespace std;

static long do_sum(vector<long> *arr, size_t start, size_t count) {
	static mutex _m;
	long sum = 0;
	for(size_t i = 0; i < count; i++){
		sum += (*arr)[start + i];
	}
	{
		lock_guard<mutex> _(_m);
		cout << "thread " << this_thread::get_id()  << ", count=" << count << ", sum=" << sum << endl;
	}
	return sum;
}
static void threadAsync() {
	# define COUNT 100000000
	vector<long> data(COUNT);
	for(size_t i = 0; i < COUNT; i++){
		data[i] = random() & 0xff;
	}

	vector< future<long> > result;
	size_t ptc = thread::hardware_concurrency() * 2;
	for(size_t batch = 0; batch < ptc; batch++) {
		size_t batch_each = COUNT / ptc;
		if (batch == ptc - 1) {
			batch_each = COUNT - (COUNT / ptc * batch);
		}
		result.push_back(async(do_sum, &data, batch * batch_each, batch_each));
	}
	long total = 0;
	for(size_t batch = 0; batch < ptc; batch++){
		total += result[batch].get();
	}
	cout << "total=" << total << endl;
}

int main(int argc, char const *argv[])
{
	// for (int i = 0; i < 100; i++) {
		threadAsync();
	// }
	
	return 0;
}