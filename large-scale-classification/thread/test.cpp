void threadYield(void){
	unsigned int procs = thread::hardware_concurrency(), // 获取物理线程数目
	i = 0;
	thread* ta = new thread( [](){
		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		for(int i = 0, m = 13; i < COUNT; i++, m *= 17){
			this_thread::yield();
		}
		gettimeofday(&t2, NULL);
		print_time(t1, t2, " with yield");
	} );
	thread** tb = new thread*[ procs ];
	for( i = 0; i < procs; i++){
		tb[i] = new thread( [](){
			struct timeval t1, t2;
			gettimeofday(&t1, NULL);
			for(int i = 0, m = 13; i < COUNT; i++, m *= 17){
				do_nothing();
			}
			gettimeofday(&t2, NULL);
			print_time(t1, t2, "without yield");
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