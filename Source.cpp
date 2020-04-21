#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
std::condition_variable cv;
std::mutex mtx; 
int table[5]{ 0,0,0,0,0};
void production(int id) {
	unsigned int index = 0;
	int value = 1;
	while (true) {
		std::unique_lock<std::mutex> ul(mtx);
		cv.wait(ul, [] {
			for (int i = 0; i < 5; i++)
				if (table[i] == 0) 
					return true;
			return false;
		});
		while (table[index % 5] != 0)index++;
		table[index % 5] = value;
		std::cout << "Producer id:" << id << " made " << value << std::endl;
		value++;
		cv.notify_all();
	}
}
void consumption(int id) {
	unsigned int index = 0;
	while (true){
		std::unique_lock<std::mutex> ul(mtx);
		cv.wait(ul, [] {
			for (int i = 0; i < 5; i++)
				if (table[i] != 0) 
					return true;
			return false;
		}
		);
		while (table[index % 5] == 0) index++;
		std::cout << "Consumer id:" << id << " consume " << table[index % 5] << std::endl;
		table[index % 5] = 0;
		cv.notify_all();
	}
}
int main(){
	std::vector<std::thread> all_thread;
	for (int i = 1; i < 4; i++)
		all_thread.push_back(std::thread(consumption, i));
	for (int i = 1; i < 4; i++)
		all_thread.push_back(std::thread(production, i));
	std::this_thread::sleep_for(std::chrono::seconds(10));
	for (auto&x : all_thread)
		x.join();
}