#include <iostream>
#include <unordered_map>
#include <string>
#include <sys/time.h>
#include <utility>
#include <iomanip>

#define MAP_ITEMS 100000

using namespace std;

int main()
{
	unordered_map<string, bool> unordermp;
	
	timeval startTime, endTime;
	
	//1.插入MAP_ITEMS个元素到map中
	gettimeofday(&startTime, NULL);
	std::string key = "https://blog.csdn.net/qq_41453285";
	for(int i = 0; i < MAP_ITEMS; ++i){
		string sub_key = to_string(i);
		unordermp.insert(std::make_pair(key + sub_key, 1));
	}
	
	gettimeofday(&endTime, NULL);
	long insert_time = (endTime.tv_sec - startTime.tv_sec)*1000 + (endTime.tv_usec-startTime.tv_usec)/1000;
	
	//2.在map中查找一个元素
	gettimeofday(&startTime, NULL);
	if( unordermp.find(key + "10000") == unordermp.end())
		std::cout << "not found!" << std::endl;
	
	gettimeofday(&endTime, NULL);
	long find_time = endTime.tv_usec - startTime.tv_usec;
	
	//3.估算当前key的平均大小
	double key_size = key.size() + to_string(MAP_ITEMS).size()/2;
	
	//4.打印相关信息
	std::cout << "Number of members  " << "key size  " << "insert time(ms)  " << "find time(us)  " << std::endl;
	std::cout << left << setw(19) << MAP_ITEMS;
	std::cout << left << setw(10) << key_size;
	std::cout << left << setw(17) << insert_time;
	std::cout << left << setw(15) << find_time << std::endl;
}
