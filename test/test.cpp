#include "test.h"
#include <iostream>
#include <variant>

using namespace std;

void Student::debug() const
{
	cout << _name << "'s age is:" << _age << endl;
}

Student::Student(const std::string & name, int age) :
	_name(name), _age(age) 
{
	// do something that you don't want users to see 
}

void Address::test() const
{
	cout << _country << "," << _province << "," << _city << endl;
}

Address::Address(const std::string & prov, const std::string & city) :
	_country("China"), _province(prov), _city(city) 
{
	// do something that you don't want users to see 
}

using ItemT = std::variant<Student, Address, int>;

inline void debug(const ItemT & item)
{
	if (auto ptr = std::get_if<Student>(&item))
	{
		cout << "a student =======" << endl;
		return ptr->debug();
	}
	else if (auto ptr = std::get_if<Address>(&item))
	{
		cout << "an address =======" << endl;
		return ptr->test();
	}
	else
	{
		cout << "invalid item =======" << endl;
	}
}

void debug(const Student & s)
{
	ItemT item = s;
	debug(item);
}

void debug(const Address & a)
{
	ItemT item = a;
	debug(item);
}

void debug(int a)
{
	ItemT item = a;
	debug(item);
}
