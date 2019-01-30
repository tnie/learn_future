关于 `std::async` 的执行策略：
- 若设置 `async` 标志，则 async 在新的执行线程（初始化所有线程局域对象后）执行可调用对象
- 若设置 `deferred` 标志，则 async 以同 `std::thread` 构造函数的方式转换可调用对象及其参数，但不产出新的执行线程。而是进行*惰性求值*：在 async 所返回的 `std::future` 上首次调用**非定时等待函数**，将导致在当前线程（不必是最初调用 `std::async` 的线程）中，以参数的**副本**调用可调用对象的**副本**。

	比如 `future::wait_for` 定时等待函数不会启动 async 执行可调用对象。

# future

future 不支持拷贝构造，但支持移动构造。

# folly/future

> Be aware that Folly is a 64-bit only library.

关于异步回调形式的接口 `int async_get(string key, std::function<void(GetReply)> callback);`，[folly/Future 手册][3] 描述如下：

> Very performant code can be written with an API like this, but for nontrivial applications the code devolves into a special kind of [spaghetti code][1] affectionately referred to as "callback hell".

关于 callback hell 的详细描述，见 [Futures for C++11 at Facebook][2]

关于线程：

> Threads are heavyweight — switching threads is inefficient, they have considerable memory overhead, and the OS will bog down if you make too many of them. The result is wasted resources, reduced throughput, and increased latency (because requests are in a queue, waiting to be serviced).

# 使用笔记

通过 `vcpkg install folly:x64-windows` 安装 folly，因为系统同时安装了多个版本的 msvc，所以其默认使用的 vc2017 编译。

在 vc2015 中使用时碰到以下问题，但在 vc2017 中却能够编译通过。是因为 vc2017 使用的运行时更新吗？可是 vc2015 和 vc2017 不是说二进制兼容的吗？！
```
error LNK2019: 无法解析的外部符号 "void * __cdecl operator new(unsigned __int64,enum std::align_val_t)
error LNK2001: 无法解析的外部符号 "void * __cdecl operator new(unsigned __int64,enum st
error LNK2019: 无法解析的外部符号 "void __cdecl operator delete(void *,unsigned __int64,enum std::alig
error LNK2001: 无法解析的外部符号 "void __cdecl operator delete(void *,unsigned __int64
error LNK2001: 无法解析的外部符号 "void __cdecl operator delete(void *,unsigned __int64,enum std::alig
```

另外，因为 vcpkg 会自动链接 `$(VcpkgRoot)debug\lib\*.lib`（大多在项目中都用不到）引入多余的库，但却链接不全。需要
```cpp
// 因为 vcpkg 链接了多余的某些库，且在 msvc 中缺失以下依赖
#pragma comment(lib, "wldap32.lib" )
#pragma comment(lib, "crypt32.lib" )
#pragma comment(lib, "Ws2_32.lib")
```

## 解决思路

卸载 folly 之后，指定 v140 工具集重新安装 `set(VCPKG_PLATFORM_TOOLSET v140)`。

~~安装时，只安装 folly::future 试验能否避免多余依赖~~。folly 并不能像 boost 那样部分安装。

卸载 boost `.\vcpkg.exe remove boost-vcpkg-helpers --recurse`


[1]:https://zh.wikipedia.org/wiki/%E9%9D%A2%E6%9D%A1%E5%BC%8F%E4%BB%A3%E7%A0%81
[2]:https://code.fb.com/developer-tools/futures-for-c-11-at-facebook/
[3]:https://github.com/facebook/folly/blob/master/folly/docs/Futures.md