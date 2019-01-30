#pragma once
#include <thread>
#include <chrono>
#include "folly/futures/Future.h"

namespace niel
{
    namespace sync
    {
        int add(int a, int b)
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
            return a + b;
        }
    }

    namespace async
    {
        template<typename T>
        using CallbackT = std::function<void(T)>;
        void add(int a, int b, CallbackT<int> cb)
        {
            std::thread([=] {
                cb(sync::add(a, b));
            }).detach();
        }
    }

    namespace future
    {
        folly::Future<int> add(int a, int b)
        {
            auto promise = std::make_shared<folly::Promise<int>>();
            std::thread([=] {
                promise->setValue(sync::add(a, b));
            }).detach();
            return promise->getFuture();
        }
    }
}
