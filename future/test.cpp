#include <iostream>
#include <future>
#include <thread>
#include <chrono>
using namespace std;

int main()
{
    auto t1 = std::chrono::steady_clock::now();
    auto task = std::async( /*launch::deferred,*/   []() {
        std::this_thread::sleep_for(3s);
    });
    // 启用 deferred 执行策略，会导致死循环
    std::future_status status;
    do {
        status = task.wait_for(std::chrono::seconds(1));
        if (status == std::future_status::deferred) {
            std::cout << "deferred\n";
        }
        else if (status == std::future_status::timeout) {
            std::cout << "timeout\n";
        }
        else if (status == std::future_status::ready) {
            std::cout << "ready!\n";
        }
    } while (status != std::future_status::ready);
    //
    std::this_thread::sleep_for(2s);
    task.get();
    auto t2 = std::chrono::steady_clock::now();
    cout << std::chrono::duration_cast<chrono::milliseconds>(t2 - t1).count() << endl;
    return 0;
}