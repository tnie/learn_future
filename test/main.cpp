#include <string>
#include <iostream>
#include "test.h"

// Note ʹ�� v140 toolset �ǲ�֧�� C++17 ���Եġ�����ʧ��
//#define __REPLACE_NEW_

using namespace std;

int main()
{
	//cout << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << endl;	// C++17 ��
	// ����
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
// ������С�����滻��
void* operator new(std::size_t sz) {
	std::printf("global op new called, size = %zu\n", sz);
	return std::malloc(sz);
}

// C++17 ��
void* operator new  (std::size_t sz, std::align_val_t al)
{
	std::printf("global op new called, size = %zu\n", sz);
	std::printf("�һ���֪����ôʹ�� ��\n");
	return std::malloc(sz);
}
#endif // __REPLACE_NEW_
