#include <future>
#include <chrono>
#include <iostream>

using namespace std;

#define __NOW_IN_MILLISECONDS_  \
chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count()

int fun_async()
{
    auto f1 = std::async/*模板函数*/(launch::async, ([] {
        this_thread::sleep_for(2s);
        return 2;
    }));
    cout << __NOW_IN_MILLISECONDS_ << "\n"  // TODO why print 09:00:02 then 09:00:00
        << f1.get() << "\n"
        << __NOW_IN_MILLISECONDS_
        << endl;
    return 0;
}

int fun_task()
{
    auto do_add = [](int a, int b) {
        this_thread::sleep_for(2s);
        return (a + b);
    };
    cout << __NOW_IN_MILLISECONDS_ << "\n"
        << do_add(2, 52) << "\n"
        << __NOW_IN_MILLISECONDS_
        << endl;

    // 包装任何 Callable 可调用目标以返回 std::future
    packaged_task<double(int, int)> task1(do_add);
    auto f1 = task1.get_future();
    task1(2, 3);
    cout << __NOW_IN_MILLISECONDS_ << "\n"
        << f1.get() << "\n"
        << __NOW_IN_MILLISECONDS_
        << endl;

    return 0;
}

int main()
{
    promise<int> p1;    //std::promise 只应当使用一次。
    auto f1 = p1.get_future();
    std::thread([&p1] {
        this_thread::sleep_for(2s);
        p1.set_value(2);    //若无共享状态或共享状态已存储值或异常，则抛出异常。
    }).detach();
    cout << __NOW_IN_MILLISECONDS_ << "\n"
        << f1.get() << "\n"
        << __NOW_IN_MILLISECONDS_
        << endl;
    try
    {
        p1.get_future();
    }
    catch (const std::exception& e)
    {
        cerr << e.what() << endl;
    }
    return 0;
}