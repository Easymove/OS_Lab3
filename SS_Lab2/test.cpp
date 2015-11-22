#include "mem_allocator.h"

#define PGS 5
#define PG_LEN 5000

using namespace std;

void random_test_run(int proc_number, int max_proc_request, int max_proc_duration){
	int time = 0;
	list<Process> working;
	list<Process> done;
	list<Process> waiting;
	MemoryAllocator mem(PGS, PG_LEN);
	int uid = 0;
	int added_procs = 0;

	while (true){
		bool actionp = false;

		for each (auto var in waiting)
		{
			int* p = (int*)mem.allocate_mem(var.mem_req);
			if (p != NULL){
				working.push_back(Process(var.mem_req, p, time, var.time, var.id));
				cout << endl << " >>> Started delayed process: id " << var.id << "; duration " << var.time << "; requested_mem " << var.mem_req << ";" << endl;

				actionp = true;
				break;
			}
		}

		for each (auto var in working)
		{
			waiting.remove(var);
		}

		if ((rand() % 100 < 10) && (added_procs < proc_number)){
			added_procs++;
			int req_mem = rand() % max_proc_request;
			int dur = rand() % max_proc_duration;
			int* p = (int*)mem.allocate_mem(req_mem);
			char* status;

			if (p != NULL){
				working.push_back(Process(req_mem, p, time, dur, uid));
				status = "working";
			}
			else{
				waiting.push_back(Process(req_mem, p, time, dur, uid));
				status = "waiting";
			}
			
			actionp = true;
			uid++;
			cout << endl << " >>> Added process (" << status << "): id " << uid << "; duration " << dur << "; requested_mem " << req_mem << ";" << endl;
		}

		for each (auto var in working)
		{
			if (var.solved(time)){
				cout << endl << " >>> Process finished: id " << var.id << "; mem_to_free " << var.mem_req << ";" << endl;
				done.push_back(var);
				mem.free_mem(var.addr);

				actionp = true;
			}
		}

		for each (auto var in done)
		{
			working.remove(var);
		}

		if (done.size() == proc_number){
			break;
		}

		if (actionp){
			cout << endl << "current time: " << time << endl;
			mem.monitor_mem(false);
		}

		_sleep(100);
		time++;
	}

	cout << endl << "All tasks are done!" << endl;
	mem.monitor_mem(true);
}

void allocator_test(){
	MemoryAllocator test = MemoryAllocator(1, 500);

	int* addr = (int*)test.allocate_mem(100);
	int* addr2 = (int*)test.allocate_mem(200);
	int* addr3 = (int*)test.allocate_mem(100);

	test.monitor_mem(true);
	test.free_mem(addr2);
	test.monitor_mem(true);
	test.defrag_mem();
	test.monitor_mem(true);
	addr2 = (int*)test.allocate_mem(300);
	test.monitor_mem(true);
}

int main(){
	random_test_run(20, 2000, 200);
}