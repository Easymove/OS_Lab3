#include "mem_allocator.h"


//MemPage definition

MemPage::MemPage(int len, int pid, int offs){
	id = pid;
	length = len;
	offset = offs;
	not_full = true;

	mem_info tmp = { 0, len };
	free_map.insert(make_pair(0, tmp));
}

void MemPage::print(bool detailed){
	cout << "  =====>" << endl;
	cout << "	page id: " << id << endl;
	if (detailed){
		cout << "	offset: " << offset << endl;
		cout << "	length: " << length << endl;
	}

	cout << "	memory map: " << endl;
	for each (pair<int,mem_info> var in mem_map)
	{
		cout << "		addr: " << var.second.addr << "; length: " << var.second.length << endl;
	}

	cout << "	free map: " << endl;
	for each (pair<int, mem_info> var in free_map)
	{
		cout << "		addr: " << var.second.addr << "; length: " << var.second.length << endl;
	}
}

int MemPage::get_max_free_len(){
	int val = -1;
	for each (pair<int, mem_info> var in free_map)
	{
		val = max(val, var.second.length);
	}
	return val;
}

int MemPage::allocate_mem(int size){
	for each (pair<int, mem_info> var in free_map)
	{
		if (var.second.length >= size){
			mem_info tmp = { var.second.addr, var.second.length };
			
			mem_info new_mem = { tmp.addr, size };
			mem_map.insert(make_pair(new_mem.addr, new_mem));
			
			mem_info new_free = { tmp.addr + size, tmp.length - size };
			free_map.erase(var.first);
			free_map.insert(make_pair(tmp.addr + size, new_free));

			return tmp.addr;
		}
	}
	return -1;
}

void MemPage::merge_free(){
	list<pair<int, int>> to_merge;

	for each (pair<int, mem_info> fst in free_map)
	{
		for each (pair<int, mem_info> snd in free_map)
		{
			if (fst.second.length + fst.second.addr == snd.second.addr){
				to_merge.push_back(make_pair(fst.second.addr, snd.second.addr));
			}
		}
	}

	to_merge.reverse();

	for each (pair<int, int> var in to_merge)
	{
		mem_info new_free = { var.first, free_map.at(var.first).length + free_map.at(var.second).length };
			
		free_map.erase(var.first);
		free_map.erase(var.second);

		free_map.insert(make_pair(var.first, new_free));
	}
}

void MemPage::free_mem(int addr){
	for each (pair<int, mem_info> var in mem_map)
	{
		if (var.first == addr){
			mem_info tmp = { var.second.addr, var.second.length };

			mem_info new_free = { tmp.addr, tmp.length };
			free_map.insert(make_pair(new_free.addr, new_free));

			mem_map.erase(var.first);
			merge_free();
			merge_free();

			return;
		}
	}
}

void MemPage::defrag_mem(){
	map<int, mem_info> new_mem_map;

	int start_addr = 0;
	for each (pair<int, mem_info> var in mem_map)
	{
		if (start_addr != var.first){
			mem_info new_mem = { start_addr, var.second.length };
			new_mem_map.insert(make_pair(start_addr, new_mem));
		}
		else{
			new_mem_map.insert(var);
		}
		start_addr += var.second.length;
	}

	mem_map.clear();
	mem_map = new_mem_map;

	free_map.clear();
	mem_info new_free = { start_addr, length - start_addr };
	free_map.insert(make_pair(start_addr, new_free));
}


//MemoryAllocator definition

void MemoryAllocator::monitor_mem(bool detailed){
	cout << "<><><><><><><><><>  MEMORY STAT  <><><><><><><><><>" << endl;

	for each (MemPage var in mem_pages)
	{
		var.print(detailed);
	}
	if (detailed){
		cout << endl << mem_pages.size() << " MEMORY PAGES;" << endl << endl;
	
	}
}

MemoryAllocator::MemoryAllocator(int pgs, int pg_len){
	mem_size = pgs * pg_len;
	memory = (char*)malloc(sizeof(char)*mem_size);
	uid = 0;
	offset = 0;
	pages = pgs;
	page_len = pg_len;

	for (int i = 0; i < pages; i++){
		mem_pages.push_back(MemPage(page_len, uid, offset));
		uid++;
		offset += page_len;
	}
}

void* MemoryAllocator::allocate_mem(int size){
	if (size <= mem_size){

		int res_addr;

		for (auto i = mem_pages.begin(); i != mem_pages.end(); i++)
		{
			MemPage* page = &(*i);
			if ((*page).get_max_free_len() >= size){
				res_addr = (*page).offset + (*page).allocate_mem(size);
				return &memory[res_addr];
			}
		}

		defrag_mem();
		
		for (auto i = mem_pages.begin(); i != mem_pages.end(); i++)
		{
			MemPage* page = &(*i);
			if ((*page).get_max_free_len() >= size){
				res_addr = (*page).offset + (*page).allocate_mem(size);
				return &memory[res_addr];
			}
		}

		return NULL;
	}

	return NULL;
}

int MemoryAllocator::get_index(int *addr){
	return (int)addr - (int)memory;
}

void MemoryAllocator::free_mem(int *addr){
	int page_id = get_index(addr) / page_len;

	for (auto i = mem_pages.begin(); i != mem_pages.end(); i++)
	{
		MemPage* var = &(*i);
		if ((*var).id == page_id){
			(*var).free_mem(get_index(addr) - (*var).offset);
			return;
		}
	}
}

void* MemoryAllocator::reallocate_mem(int *addr, int new_size){
	free_mem(addr);
	return allocate_mem(new_size);
}

void MemoryAllocator::defrag_mem(){
	for (auto i = mem_pages.begin(); i != mem_pages.end(); i++)
	{
		MemPage* var = &(*i);
		(*var).defrag_mem();
	}
}

Process::Process(int mem, int* add, int st, int t, int uid){
	id = uid;
	mem_req = mem;
	addr = add;
	start = st;
	time = t;
}

bool Process::operator==(const Process& b){
	return this->id == b.id;
}

bool Process::solved(int t){
	return t == time + start;
}