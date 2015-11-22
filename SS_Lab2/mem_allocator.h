#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

using namespace std;

struct mem_info{
	int addr;
	int length;
};

class MemPage{
public:
	MemPage(int len, int pid, int offs);
	~MemPage(){};

	int id;
	int length;
	int offset;
	bool not_full;
	
	void print(bool detailed);
	int get_max_free_len();
	int allocate_mem(int size);
	void free_mem(int addr);
	void defrag_mem();

private:
	map<int, mem_info> mem_map;
	map<int, mem_info> free_map;

	void merge_free();
};


class MemoryAllocator
{
public:
	MemoryAllocator(int pgs, int pg_len);
	~MemoryAllocator(){};

	void monitor_mem(bool detailed);
	void* allocate_mem(int bytes);
	void* reallocate_mem(int *addr, int new_size);
	void free_mem(int *addr);
	void defrag_mem();

private:
	int offset;
	int uid;
	int mem_size;
	list<MemPage> mem_pages;
	int pages;
	int page_len;
	char* memory;

	int get_index(int *addr);
};

class Process
{
public:
	int id;
	int start;
	int time;
	int* addr;
	int mem_req;

	Process(int mem, int* add, int st, int t, int uid);
	~Process(){};

	bool solved(int t);
	bool operator==(const Process& b);
};