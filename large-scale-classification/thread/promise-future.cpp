#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <future>
#include <utility>
#include <time.h>
#include <deque>

using namespace std;

#define COUNT 10000000
int a, b, c, d;


int do_sum(vector<int> *arr, size_t start, size_t count) {
	// this_thread::sleep_for( chrono::seconds(3) );
	static mutex _m;
 	int sum = 0;
 	for(size_t i = 0; i < count; i++){
 		sum += (*arr)[start + i];
 	}

	{
	lock_guard<mutex> _(_m);
	cout << "thread " << this_thread::get_id() << ", count=" << count << ", sum=" << sum << endl;
	}
	return sum;
}

void thread_future_promise() {
	vector<int> data(COUNT);
	for(size_t i = 0; i < COUNT; i++){
 		data[i] = 1;
 	}

 	long last_sum = 0;
	size_t ptc = thread::hardware_concurrency();
	thread** tb = new thread*[ptc];
	vector<packaged_task<int(vector<int> *, int, int)> > task_v;
	vector<future<int> > future_v;

 	for (int batch = 0; batch < ptc; batch++) {
 		size_t batch_each = COUNT / ptc, start = batch * batch_each;
	 	if (batch == ptc - 1){
	 		batch_each = COUNT - (COUNT / ptc * batch);
	 	}
	 	packaged_task<int(vector<int> *, int, int)> sub_pt(do_sum);
	 	future_v.push_back(sub_pt.get_future());
	 	task_v.push_back(move(sub_pt));
	 	tb[batch] = new thread(move(task_v[batch]), &data, start, batch_each);

 	}

	
	for (int batch = 0; batch < ptc; batch++) {
		tb[batch]->join();
		cout << future_v[batch].get() << endl;
		delete tb[batch];
	}
	delete tb;
	// task_v.clear();
}

static mutex g_mutex;
static void threadPackagedTask(){
	auto run = [=](int index){ 
		{
			lock_guard<mutex> _(g_mutex);
			cout << "tasklet " << index << endl;
		}
		cout << this_thread::get_id() << endl;
		double s = 0.1;
		for (int i = 0; i < 100000000; i++) {
			s += (s * 0.1) - s * 0.00002 + s;
		}
		// this_thread::sleep_for( chrono::seconds(10) );
		return index * 1000;
 	};
	packaged_task<int(int)> pt1(run);
	packaged_task<int(int)> pt2(run);
	packaged_task<int(int)> pt3(run);
	packaged_task<int(int)> pt4(run);
	thread t1([&](){pt1(2);});
	// t1.join();
	thread t2([&](){pt2(2);} );
	// t2.join();
	thread t3([&](){pt3(3);} );
	// t3.join();
	thread t4([&](){pt4(4);} );
	// t4.join();
	int f1 = pt1.get_future().get();
	int f2 = pt2.get_future().get();
	cout << "task result=" << f1 << endl;
	cout << "task result=" << f2 << endl;
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

void threadYield(void){
	unsigned int procs = thread::hardware_concurrency(), // 获取物理线程数目
	i = 0;
	thread* ta = new thread( [](){
		struct timeval t1, t2;
		// gettimeofday(&t1, NULL);
		for(int i = 0, m = 13; i < COUNT; i++, m *= 17){
			this_thread::yield();
		}
		// gettimeofday(&t2, NULL);
		// print_time(t1, t2, " with yield");
	} );
	thread** tb = new thread*[ procs ];
	for( i = 0; i < procs; i++){
		tb[i] = new thread( [](){
			struct timeval t1, t2;
			// gettimeofday(&t1, NULL);
			for(int i = 0, m = 13; i < COUNT; i++, m *= 17){
				// do_nothing();
			}
			// gettimeofday(&t2, NULL);
			// print_time(t1, t2, "without yield");
		});
	}
	ta->join();
	delete ta;
	for( i = 0; i < procs; i++){
		tb[i]->join();
		delete tb[i];
	};
	delete tb;
}

int main() {
	int times = 500;
	while(times--) {
		thread_future_promise();
	}
	// while (times--) {
	// 	threadPackagedTask();
	// }

	// threadYield();
	return 0;
}