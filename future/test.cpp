#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include "basic.h"
using namespace std;

// 因为 vcpkg 链接了多余的某些库，且在 msvc 中缺失以下依赖
#pragma comment(lib, "wldap32.lib" )
#pragma comment(lib, "crypt32.lib" )
#pragma comment(lib, "Ws2_32.lib")

void folly_future()
{
    using namespace niel::async;
    add(1, 2, [](int s1) {
        add(s1, 3, [](int s2) {
            cout << "sum is:" << s2 << endl;
        });
    });

    using namespace niel::future;
    auto future = add(1, 2).thenValue([](int s1) {
        add(s1, 4).thenValue([](int s2){
            cout << "sum is:" << s2 << endl;
        });
    });
}


void std_future()
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
}

int main()
{
    folly_future();
    getchar();
    return 0;
}