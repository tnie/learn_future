关于 `std::async` 的执行策略：
- 若设置 `async` 标志，则 async 在新的执行线程（初始化所有线程局域对象后）执行可调用对象
- 若设置 `deferred` 标志，则 async 以同 `std::thread` 构造函数的方式转换可调用对象及其参数，但不产出新的执行线程。而是进行*惰性求值*：在 async 所返回的 `std::future` 上首次调用**非定时等待函数**，将导致在当前线程（不必是最初调用 `std::async` 的线程）中，以参数的**副本**调用可调用对象的**副本**。

	比如 `future::wait_for` 定时等待函数不会启动 async 执行可调用对象。

# future

future 不支持拷贝构造，但支持移动构造。

# folly/future

> Folly is a library of C++14 components designed with practicality and efficiency in mind.
> 
> Be aware that Folly is a 64-bit only library.

关于异步回调形式的接口 `int async_get(string key, std::function<void(GetReply)> callback);`，[folly/Future 手册][3] 描述如下：

> Very performant code can be written with an API like this, but for nontrivial applications the code devolves into a special kind of [spaghetti code][1] affectionately referred to as "callback hell".

关于 callback hell 的详细描述，见 [Futures for C++11 at Facebook][2]

关于线程：

> Threads are heavyweight — switching threads is inefficient, they have considerable memory overhead, and the OS will bog down if you make too many of them. The result is wasted resources, reduced throughput, and increased latency (because requests are in a queue, waiting to be serviced).

# 使用笔记

通过 `vcpkg install folly:x64-windows` 安装 folly，因为系统同时安装了多个版本的 msvc，所以其默认使用的 vc2017 编译。

在 vc2015 中使用时碰到以下问题，但在 vc2017 中却能够编译通过。

```
error LNK2019: 无法解析的外部符号 "void * __cdecl operator new(unsigned __int64,enum std::align_val_t)
error LNK2001: 无法解析的外部符号 "void * __cdecl operator new(unsigned __int64,enum st
error LNK2019: 无法解析的外部符号 "void __cdecl operator delete(void *,unsigned __int64,enum std::alig
error LNK2001: 无法解析的外部符号 "void __cdecl operator delete(void *,unsigned __int64
error LNK2001: 无法解析的外部符号 "void __cdecl operator delete(void *,unsigned __int64,enum std::alig
```

是因为 vc2017 使用的运行时新吗？ ~~可是 vc2015 和 vc2017 不是说二进制兼容的吗~~？！ ~~是因为 vc2107 支持 C++17 新标准，而 vc2015 不支持~~ 和新标准有关：[new 表达式][9] 和定义在 `<new>` 头文件中 [operator new, operator new[]][10] 函数是两回事

```cpp
// 虽然是老面孔的单词 new/delete，但却是函数，且后两者是新标准 C++17 内容！
void* operator new  ( std::size_t count );				// (1)	
void* operator new[]( std::size_t count );				// (2)	
void* operator new  ( std::size_t count, std::align_val_t al);		// (3)	(C++17 起)
void* operator new[]( std::size_t count, std::align_val_t al);		// (4)	(C++17 起)
```

但并非通过 v141 使用新特性<sup>1</sup>封装库之后就一定不能使用 v140 调用：test.lib 和 call_test.exe：
- test.lib 内部使用了 `std::variant` C++17 类型，使用 v141 编译成动态库；
- call_test.lib 使用 v140 链接前者的导入库，执行时调用前者的动态库，输出正常； 

关于 vc2015 以及 17 版本的二进制兼容：[C++ Binary Compatibility between Visual Studio 2015 and Visual Studio 2017][4]，其中明确提到了有两种场景是不保证（NOT guaranteed）其二进制兼容的。

> 1. When static libraries or object files are compiled with the `/GL` compiler switch.
> 
> 2. When consuming libraries built with a **toolset** whose version is greater than the **toolset used to compile and link** the application. 

我之前测试 vc2015 & 17 二进制兼容时，因为例子特别简单，所以巧合地（虽然 microsoft 不保证）两者二进制兼容。即便 test.lib & call_test.exe 前者使用了新标准特性，却依然能够正确输出。可是在 folly 库却失败了。

sup1 此处特指 v140 不支持但 v141 支持的 C++ 语言特性，并非特指 C++11 或 C++14 或 C++17 等

### 小问题

另外，因为 vcpkg 会自动链接 `$(VcpkgRoot)debug\lib\*.lib`（大多在项目中都用不到）引入多余的库，但却链接不全。需要
```cpp
// 因为 vcpkg 链接了多余的某些库，且在 msvc 中缺失以下依赖
#pragma comment(lib, "wldap32.lib" )
#pragma comment(lib, "crypt32.lib" )
#pragma comment(lib, "Ws2_32.lib")
```

##  Binary Compatibility

此小节在 2019-10-17 新增

《C++ Binary Compatibility between Visual Studio 2015 and Visual Studio 2017》 原贴在微软网站上已经找不到了，但在 Stack Overflow 上仍然可见到有关的 [描述][113] 和 [讨论][114] 。其 URL 下的文章已经更新到 vs2019，现在只提到了 `/GL` 的例外，去掉了第二点，但又没有明确地表示

> This means that if you have third-party library that was built with Visual Studio ~~2015~~ 2017 or Visual Studio 2019, you don't have to recompile it in order to consume it from an application that is built with Visual Studio 2015 ~~2017 or Visual Studio 2019~~.

再结合之前的测试 demo，推测二进制兼容其实是保证的，只是如果接口定义（即头文件中的声明）存在差异，编译都失败了，谈二进制兼容没有意义。

我打算跳坑了，因为“卸载 vc2017/2019；使用 vcpkg 安装 vc2015 的库；重新安装 vc2017/2019；安装 vc2019 的库” 操作反人类。先跳下去，撞了南墙再说， 还有 [同道中人](https://github.com/robotology-playground/robotology-additional-dependencies/issues/3#issuecomment-459981396)

## 解决思路（无路可走）

虽然说 folly 是 C++14 的库，但 vc2015 部分支持 C++14 特性，所以尝试之后再定，撞了南墙再回头。

卸载 folly 之后，指定 v140 工具集重新安装 `set(VCPKG_PLATFORM_TOOLSET v140)`。安装 zlib 竟然失败了，失败了：错误情况与 [vcpkg issue #1833][5] 相同，但此 issue 中提到的 [改变 vc2017 的语言][6] 的方案并不能解决我的问题。

测试 vcpkg + vc2015 能否正确安装 zlib？在 [vcpkg issue #766][11] 中 [ras0219-msft][111] 提到

> We don't currently have an easy, supported way to opt out of VS2017 (Using VS2015 when VS2017 is available). 

给出的 set `VCPKG_PLATFORM_TOOLSET ` 的 workaround 也有大的缺陷：

> This will work for CMake-based ports that don't use Ninja and possibly others. Notably, this will _not_ change `boost`.

[jacobblock][112] 也提到了 vcpkg + vc2017 + v140 toolset 下 zlib 安装失败的问题：

> I was basically setting `VCPKG_PLATFORM_TOOLSET` to v140 in my triplet file and getting an immediate build error with zlib. The build was still trying to be performed with VS2017 (and works if I don't specify the `VCPKG_PLATFORM_TOOLSET`). 
> 
> Forcing the bootstrap to find VS2015 instead of VS2017 fixed the error;

仔细查看 vcpkg/buildtrees/zlib 下的日志，发现它是使用 Ninja 编译的，正中 vcpkg + vc2017 + v140 的例外！

~~安装时，只安装 folly::future 试验能否避免多余依赖~~。folly 并不能像 boost 那样部分安装。

卸载 boost `.\vcpkg.exe remove boost-vcpkg-helpers --recurse`

不通过 vcpkg，使用 cmake 从头做起呢？作为最后的方案备选，暂不考虑。

用 vc2017 & c++17(v141 toolset) 生成的链接库，一定不能在 vc2015(v140 toolset) 环境中调用吗？有的可以，有的不可以，也就是微软提到的 NOT guaranteed。

**结论**：老老实实地使用 vcpkg+vc2017+v141，想要 vcpkg+vc2017+v140 不现实，毕竟 vcpkg 安装项目好多都是使用的 Ninja。 

## toolset

老是在说工具集，我们在使用新版本 vc2017 打开 vc2015 的项目时，会接触到两个概念：Windows SDK Version, Platform Toolset。

但两者是什么意义？有什么关联？

[MSBuild Toolset (ToolsVersion)][7]（和 msvc toolset 近似，但应该不是同一概念）叙述：

> MSBuild uses a Toolset of tasks, targets, and tools to build an application. Typically, a MSBuild Toolset includes a `microsoft.common.tasks` file, a `microsoft.common.targets` file, and **compilers** such as csc.exe and vbc.exe. Most Toolsets can be used to compile applications to more than one version of the .NET Framework and **more than one system platform**. 

所以，toolset 至少会包含编译器、链接器等生成应用程序的关键工具，但 IDE 中语法检测、跳转与色彩主体等肯定不是。如此，那对于新标准的支持（eg vc2015 不支持 c++14）也要落到 toolset 上，所以使用 v140 toolset 的 vc2017 也无法使用 c++14 呗？ 

> To change the target platform toolset, you must have the associated version of Visual Studio or the Windows Platform SDK installed. [来源][8]

扩展阅读：[MSBuild (Visual C++)](https://docs.microsoft.com/zh-cn/cpp/build/msbuild-visual-cpp?view=vs-2017)

[1]:https://zh.wikipedia.org/wiki/%E9%9D%A2%E6%9D%A1%E5%BC%8F%E4%BB%A3%E7%A0%81
[2]:https://code.fb.com/developer-tools/futures-for-c-11-at-facebook/
[3]:https://github.com/facebook/folly/blob/master/folly/docs/Futures.md
[4]:https://docs.microsoft.com/en-us/cpp/porting/binary-compat-2015-2017?view=vs-2017
[5]:https://github.com/Microsoft/vcpkg/issues/1833
[6]:https://stackoverflow.com/a/43762131/6728820
[7]:https://docs.microsoft.com/en-us/visualstudio/msbuild/msbuild-toolset-toolsversion?view=vs-2017
[8]:https://docs.microsoft.com/zh-cn/cpp/build/how-to-modify-the-target-framework-and-platform-toolset?view=vs-2017
[9]:https://zh.cppreference.com/w/cpp/language/new
[10]:https://zh.cppreference.com/w/cpp/memory/new/operator_new
[11]:https://github.com/Microsoft/vcpkg/issues/766
[111]:https://github.com/Microsoft/vcpkg/issues/766#issuecomment-285918864
[112]:https://github.com/Microsoft/vcpkg/issues/766#issuecomment-356810721
[113]:https://stackoverflow.com/questions/53185019/questions-about-binary-compatibility-between-visual-studio-2015-and-visual-stud
[114]:https://stackoverflow.com/questions/53187152/is-the-official-binary-incompatibility-between-vs2017-and-vs2015-app-vs-dll-acc
