// DataIO.cpp : Defines the entry point for the console application.
//

#undef NDEBUG

#include <stdio.h>
#include <nark/io/DataIO.hpp>
#include <nark/io/MemStream.hpp>

#undef NDEBUG

using namespace nark;
using namespace std;

struct MyData1
{
	uint32_t a, b, c, d;

//	DATA_IO_LOAD_SAVE(MyData1, &a&b&c&d)

	template<class DataIO> void load(DataIO& dio)	    { dio &a&b&c&d; }
	template<class DataIO> void save(DataIO& dio) const { dio &a&b&c&d; }
	DATA_IO_OPTIMIZE_ELEMEN_LOAD(MyData1, &a&b&c&d)
	DATA_IO_OPTIMIZE_ELEMEN_SAVE(MyData1, &a&b&c&d)
	DATA_IO_OPTIMIZE_VECTOR_LOAD(MyData1, &a&b&c&d)
	DATA_IO_OPTIMIZE_VECTOR_SAVE(MyData1, &a&b&c&d)
	DATA_IO_OPTIMIZE_ARRAY__LOAD(MyData1, &a&b&c&d)
	DATA_IO_OPTIMIZE_ARRAY__SAVE(MyData1, &a&b&c&d)
	DATA_IO_REG_LOAD(MyData1)
	DATA_IO_REG_SAVE(MyData1)
	DATA_IO_OPTIMIZE_ELEMEN_LOAD_REG(friend void, DATA_IO_IDENTITY, MyData1)
	DATA_IO_OPTIMIZE_ELEMEN_SAVE_REG(friend void, DATA_IO_IDENTITY, MyData1)
	DATA_IO_OPTIMIZE_VECTOR_LOAD_REG(friend void, MyData1, MyData1)
	DATA_IO_OPTIMIZE_VECTOR_SAVE_REG(friend void, MyData1, MyData1)
	DATA_IO_OPTIMIZE_ARRAY__LOAD_REG(friend void, MyData1, MyData1)
	DATA_IO_OPTIMIZE_ARRAY__SAVE_REG(friend void, MyData1, MyData1)
};
namespace nark {
DataIO_IsDump_TypeTrue1(MyData1)
}

struct MyData2
{
	uint32_t a, b, c, d;
};
// &a&c&b&d is not in true seq, raise assert
//DATA_IO_LOAD_SAVE_E(MyData2, &a&c&b&d)
DATA_IO_LOAD_SAVE_E(MyData2, &a&b&c&d)

struct MyData3
{
	uint32_t a, b, c, d;
};
DATA_IO_LOAD_SAVE_EV(MyData3, 2, &a&b&c&d)

template<class Iter>
void gen_rand(Iter first, Iter last)
{
	for (Iter i = first; i != last; ++i)
	{
		i->a = rand();
		i->b = rand();
		i->c = rand();
		i->d = rand();
	}
}

template<class Iter>
void gen_rand_pair(Iter first, Iter last)
{
	for (Iter i = first; i != last; ++i)
	{
		i->first = rand();
		i->second = rand();
	}
}

struct Less
{
	template<class T>
	bool operator()(const T& x, const T& y) const
	{
		return memcmp(&x, &y, sizeof(T)) < 0;
	}
};

struct Equal
{
	template<class T>
	bool operator()(const T& x, const T& y) const
	{
		return memcmp(&x, &y, sizeof(T)) == 0;
	}
};

int main(int argc, char* argv[])
{
	printf("sizeof(vector<int>)=%d\n", (int)sizeof(vector<int>));
	int count = 1000;
	vector<int> v1, w1;
	vector<pair<double,double> > v2, w2;
	vector<MyData1> v3, w3;
	vector<MyData2> v4, w4;
	vector<MyData3> v5, w5;
	vector<pair<uint64_t,double> > v6, w6;

	MyData1 arr[100], arr2[100];
	gen_rand(arr, arr+100);
	v1.resize(count);
	v2.resize(count);
	v3.resize(count);
	v4.resize(count);
	v5.resize(count);
	v6.resize(count);

	for (int i = 0; i != count; ++i) v1[i] = rand();

	gen_rand_pair(v2.begin(), v2.end());
	gen_rand(v3.begin(), v3.end());
	gen_rand(v4.begin(), v4.end());
	gen_rand(v5.begin(), v5.end());
	gen_rand_pair(v6.begin(), v6.end());

	{
		NativeDataOutput<AutoGrownMemIO> oar;
		NativeDataInput<MemIO> iar;
	#include "body.h"
	}

	{
		PortableDataOutput<AutoGrownMemIO> oar;
		PortableDataInput<MemIO> iar;
	#include "body.h"
	}

	{
		NativeNoVarIntOutput<AutoGrownMemIO> oar;
		NativeNoVarIntInput<MemIO> iar;
	#include "body.h"
	}

	{
		PortableNoVarIntOutput<AutoGrownMemIO> oar;
		PortableNoVarIntInput<MemIO> iar;
	#include "body.h"
	}

	return 0;
}

