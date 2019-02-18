#pragma once
#include <memory>
#include <string>

#ifndef __API_DECLSPEC
#ifdef _WINDLL
#define __API_DECLSPEC    __declspec(dllexport)
#elif defined(__SOURCE_TEST)
#define __API_DECLSPEC
#else
#define __API_DECLSPEC    __declspec(dllimport)
#define __STR_API_DECLSPEC "__declspec(dllimport)"
#endif
#endif // !__API_DECLSPEC

#ifndef __STR_API_DECLSPEC
#define __STR_API_DECLSPEC ""
#endif // !__STR_API_DECLSPEC

class Student
{
public:
	__API_DECLSPEC static std::shared_ptr<Student> another(const std::string & name, int age)
	{
		//return std::make_shared<Student>(name, age);
		return std::shared_ptr<Student>(new Student(name, age));
	}
	void debug() const;
private:
	Student() = default;
	Student(const std::string & name, int age);
	std::string _name;
	int _age;
};

class Address
{
public:
	__API_DECLSPEC static std::shared_ptr<Address> another(const std::string& prov, const std::string& city)
	{
		return std::shared_ptr<Address>(new Address(prov, city));
	}
	void test() const;
private:
	Address() = default;
	Address(const std::string& prov, const std::string& city);
	std::string _country;
	std::string _province;
	std::string _city;
};

__API_DECLSPEC void debug(const Student&);
__API_DECLSPEC void debug(const Address&);
__API_DECLSPEC void debug(int);

