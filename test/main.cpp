#include <string>
#include <iostream>
#include "test.h"

// Note 使用 v140 toolset 是不支持 C++17 特性的。编译失败
//#define __REPLACE_NEW_

using namespace std;

int main()
{
	//cout << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << endl;	// C++17 起
	// 常用
	auto ptr = new int(2);
	cout << *ptr << endl;
	delete ptr;
	
	ptr = new int[100]{1,2,3,4,5};
	cout << *(ptr + 3) << endl;
	delete[] ptr;

	//
	cout << __STR_API_DECLSPEC << endl;
	{
		auto ptr = Student::another("niel", 20);
		debug(*ptr);
	}
	{
		auto ptr = Address::another("hebei", "shijiazhuang");
		debug(*ptr);
	}
	debug(1208);
}


#ifdef __REPLACE_NEW_
#include <new>
// 函数最小集的替换：
void* operator new(std::size_t sz) {
	std::printf("global op new called, size = %zu\n", sz);
	return std::malloc(sz);
}

// C++17 起
void* operator new  (std::size_t sz, std::align_val_t al)
{
	std::printf("global op new called, size = %zu\n", sz);
	std::printf("我还不知道怎么使用 哈\n");
	return std::malloc(sz);
}
#endif // __REPLACE_NEW_
