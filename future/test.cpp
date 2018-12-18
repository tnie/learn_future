#include <iostream>
#include <future>
#include <thread>
#include <chrono>
using namespace std;

int main()
{
    auto t1 = std::chrono::steady_clock::now();
    auto task = std::async( launch::deferred,   []() {
        std::this_thread::sleep_for(1s);
    });
    std::this_thread::sleep_for(2s);
    task.get();
    auto t2 = std::chrono::steady_clock::now();
    cout << std::chrono::duration_cast<chrono::milliseconds>(t2 - t1).count() << endl;
    return 0;
}