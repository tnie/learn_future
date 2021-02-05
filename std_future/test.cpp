#include <future>
#include <chrono>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace std;

#define __NOW_IN_MILLISECONDS_  \
chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count()

int fun_async()
{
    auto f1 = std::async/*ģ�庯��*/(launch::async, ([] {
        this_thread::sleep_for(2s);
        return 2;
    }));
    {
        auto t1 = __NOW_IN_MILLISECONDS_;
        auto n = f1.get();
        auto t2 = __NOW_IN_MILLISECONDS_;
        spdlog::info("{}\n {}\n {} ", t1, n, t2);
    }
    //https://blog.csdn.net/me4weizhen/article/details/52541325 ��������ִ��˳�� �� <- ��
    //spdlog::info("{}\n {}\n {} ", __NOW_IN_MILLISECONDS_, f1.get(), __NOW_IN_MILLISECONDS_);
    //cout << __NOW_IN_MILLISECONDS_ << "\n"  // TODO why print 09:00:02 then 09:00:00
    //    << f1.get() << "\n"
    //    << __NOW_IN_MILLISECONDS_
    //    << endl;
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

    // ��װ�κ� Callable �ɵ���Ŀ���Է��� std::future
    packaged_task<double(int, int)> task1(do_add);
    auto f1 = task1.get_future();
    task1(2, 3);
    cout << __NOW_IN_MILLISECONDS_ << "\n"
        << f1.get() << "\n"
        << __NOW_IN_MILLISECONDS_
        << endl;

    return 0;
}

#include <algorithm>
int main()
{
    future<void> f3;
    for (size_t i = 0; i < 5; i++)
    {
        f3 = std::async(launch::async, [i]() {
            this_thread::sleep_for(5s);
            spdlog::info("niel: {}", i);
        });
        // �ص���������ͬʱ����������Σ�
        //�������ú��ƶ���ֵ�����ӳٵ� f3 ֮ǰ��������ɺ�ŷ���
    }
    auto f2 = std::async(launch::async, []() {
        this_thread::sleep_for(3s);
        spdlog::info("liyw");
    });
    return fun_async();
    promise<int> p1;    //std::promise ֻӦ��ʹ��һ�Ρ�
    auto f1 = p1.get_future();
    std::thread([&p1] {
        this_thread::sleep_for(2s);
        p1.set_value(2);    //���޹���״̬����״̬�Ѵ洢ֵ���쳣�����׳��쳣��
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