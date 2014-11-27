#if defined(_WIN32) || defined(_WIN64)
#  define WIN32_LEAN_AND_MEAN
#  include <Windows.h>
#else
#  include <time.h>
#endif
#include <fstream>
#include <nark/io/DataIO.hpp>
#include <nark/io/StreamBuffer.hpp>
#include <nark/io/FileStream.hpp>
#include <nark/io/MemStream.hpp>
#include <nark/util/profiling.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <nark/io/var_int_boost_serialization.hpp>

//Makefile:LDFLAGS:-lboost_serialization

using namespace std;
using namespace boost::archive;
using namespace nark;

namespace nark {
struct MyData1
{
	uint32_t a, b, c;
	uint32_t d[5];
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int/*version*/)
    {
        ar & a & b & c & d;
    }
#if 0
	DATA_IO_LOAD_SAVE(MyData1, &a&b&c&d)
#else
 	template<class DataIO> void load(DataIO& dio)	    { dio &a&b&c&d; }
 	template<class DataIO> void save(DataIO& dio) const { dio &a&b&c&d; }
	DATA_IO_GEN_DUMP_TYPE_TRAITS(MyData1, &a&b&c&d)
 	DATA_IO_OPTIMIZE_ELEMEN_LOAD(MyData1, &a&b&c&d)
 	DATA_IO_OPTIMIZE_ELEMEN_SAVE(MyData1, &a&b&c&d)
 	DATA_IO_OPTIMIZE_VECTOR_LOAD(MyData1, &a&b&c&d)
 	DATA_IO_OPTIMIZE_VECTOR_SAVE(MyData1, &a&b&c&d)
 	DATA_IO_OPTIMIZE_ARRAY__LOAD(MyData1, &a&b&c&d)
 	DATA_IO_OPTIMIZE_ARRAY__SAVE(MyData1, &a&b&c&d)
	DATA_IO_GEN_DUMP_TYPE_TRAITS_REG(friend, MyData1, MyData1)
 	DATA_IO_OPTIMIZE_ELEMEN_LOAD_REG(friend, DATA_IO_IDENTITY, MyData1)
 	DATA_IO_OPTIMIZE_ELEMEN_SAVE_REG(friend, DATA_IO_IDENTITY, MyData1)
 	DATA_IO_OPTIMIZE_VECTOR_LOAD_REG(friend, MyData1, MyData1)
 	DATA_IO_OPTIMIZE_VECTOR_SAVE_REG(friend, MyData1, MyData1)
 	DATA_IO_OPTIMIZE_ARRAY__LOAD_REG(friend, MyData1, MyData1)
 	DATA_IO_OPTIMIZE_ARRAY__SAVE_REG(friend, MyData1, MyData1)
	DATA_IO_REG_LOAD(MyData1)
 	DATA_IO_REG_SAVE(MyData1)
#endif
};
//DATA_IO_GEN_DUMP_TYPE_TRAITS_REG(inline, MyData1, MyData1)
DataIO_IsDump_TypeTrue1(MyData1)

using nark::MyData1;

struct MyData2
{
	uint32_t a, b, c, d;
	MyData1 e;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int/*version*/)
    {
        ar & a & b & c & d & e;
    }
};
DATA_IO_LOAD_SAVE_E(MyData2, &a&b&c&d&e)
} // namespace nark

struct MyData3
{
	uint32_t a, b, c;
	uint32_t d;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int/*version*/)
    {
        ar & a & b & c & d;
    }
};
//DATA_IO_LOAD_SAVE_EV(MyData3, 2, &a&b&c&d)
DATA_IO_DUMP_RAW_MEM_E(MyData3)

struct VarIntD
{
	var_uint32_t a, b, c, d, e, f;

	VarIntD()
	{
		a = 127;
		b = 128;
		c = 128*128;
		d = 128*128*128;
		e = 128*128*128*128;
		f = 1;
	}

// 如果使用var_int存储小整数会节省点空间
// 无需使用 as_var_int, 它们本身已是 var_int
// 	DATA_IO_LOAD_SAVE(VarIntD,
// 		&as_var_int(a)
// 		&as_var_int(b)
// 		&as_var_int(c)
// 		&as_var_int(d))

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int/*version*/)
    {
        ar & a & b & c & d & e & f;
    }
};
DATA_IO_LOAD_SAVE_E(VarIntD, &a&b&c&d&e&f)

struct TimeCost
{
	uint64_t d0, d1, d2, d3, d4, d5, d6;

	TimeCost() { d0 = 0; d1 = 0; d2 = 0; d3 = 0; d4 = 0; d5 = 0; d6 = 0; }
};

typedef pair<MyData2, MyData3> MyData23;
struct Bench
{
	nark::profiling pf;
	int loop, count;
	vector<pair<int,int> > v0;
	vector<MyData1> v1;
	vector<string> v2;
	map<int, string> v3;
	vector<MyData23> v4;

	TimeCost bdo, bdi;
#if defined(_WIN32) || defined(_WIN64)
	LARGE_INTEGER freq;
#endif

Bench(int loop, int count)
: loop(loop), count(count)
{
	for (int i = 0; i < count; ++i)
	{
		char szbuf[32];
		int x = rand();
	//	itoa(x, szbuf, 10);
	// 输出16字节长的 string 是为了使存储格式的地址对齐
		sprintf(szbuf, "%016d", x);
		v0.push_back(make_pair(x, x));
		v2.push_back(szbuf);
		v3.insert(make_pair(x, szbuf));

		MyData1 md1;
		md1.a = md1.b = md1.c = md1.d[0] = x;
		v1.push_back(md1);

		MyData2 md2;
		MyData3 md3;
		md2.a = md2.b = md2.c = md2.d = x;
		md3.a = md3.b = md3.c = md3.d = x;
		v4.push_back(make_pair(md2, md3));
	}
}



void print_report(const char* prefix, const TimeCost& d, const TimeCost& bd)
{
	int maplen = v3.size();
	printf("%s: loop=%d, time[nark, boost, ratio=b/f] in us\n", prefix, loop);
	printf("    vector<pair<int,int> >.size=%5d, time[%10.3f, %10.3f, %8.3f]\n", count, pf.uf(d.d0), pf.uf(bd.d0), (double)bd.d0/d.d0);
	printf("    vector<MyData1>       .size=%5d, time[%10.3f, %10.3f, %8.3f]\n", count, pf.uf(d.d1), pf.uf(bd.d1), (double)bd.d1/d.d1);
	printf("    vector<string>        .size=%5d, time[%10.3f, %10.3f, %8.3f]\n", count, pf.uf(d.d2), pf.uf(bd.d2), (double)bd.d2/d.d2);
	printf("    map<int,string>       .size=%5d, time[%10.3f, %10.3f, %8.3f]\n",maplen, pf.uf(d.d3), pf.uf(bd.d3), (double)bd.d3/d.d3);
	printf("    loop{MyData1 },  loop count=%5d, time[%10.3f, %10.3f, %8.3f]\n", count, pf.uf(d.d4), pf.uf(bd.d4), (double)bd.d4/d.d4);
	printf("    loop{VarIntD },  loop count=%5d, time[%10.3f, %10.3f, %8.3f]\n", count, pf.uf(d.d5), pf.uf(bd.d5), (double)bd.d5/d.d5);
	printf("    vector<MyData23>,     .size=%5d, time[%10.3f, %10.3f, %8.3f]\n", count, pf.uf(d.d6), pf.uf(bd.d6), (double)bd.d6/d.d6);
	printf("\n");
}

	template<class ArchiveT>
// #ifdef _MSC_VER
// 	__forceinline
// #endif
	void test_serialize(ArchiveT& ar, TimeCost& d)
	{
		uint64_t c0, c1, c2, c3, c4, c5, c6, c7;

		c0 = pf.now();
		ar & v0;
		c1 = pf.now();
		ar & v1;
		c2 = pf.now();
		ar & v2;
		c3 = pf.now();
		ar & v3;
		c4 = pf.now();

		for (size_t i=0, n=v0.size(); i < n; ++i)
		{
			MyData1 md1;
			ar & md1;
		}
		c5 = pf.now();

		for (size_t i=0, n=v0.size(); i < n; ++i)
		{
			VarIntD vid;
			ar & vid;
		}
		c6 = pf.now();

		ar & v4;
		c7 = pf.now();

		d.d0 += c1 - c0;
		d.d1 += c2 - c1;
		d.d2 += c3 - c2;
		d.d3 += c4 - c3;
		d.d4 += c5 - c4;
		d.d5 += c6 - c5;
		d.d6 += c7 - c6;
	}

	void run_boost()
	{
		size_t data_size;
		{
			std::ostringstream oss;
			for (int i = 0; i < loop; ++i)
			{
				oss.seekp(0);
				binary_oarchive ar(oss);
			//	test_serialize(ar, bdo);
				TimeCost& d = bdo;
#ifdef _DEBUG
			test_serialize(ar, d);
#else
			#include "test_body.h"
#endif
			}
			print_report("boost_bin_save", bdo, bdo);
			std::ofstream file("boost.bin", ios::binary);
			std::string const& mem = oss.str();
			file.write(mem.data(), mem.size());
			data_size = mem.size();
		}
		{
			std::ifstream file("boost.bin", ios::binary);
			std::string str;
			str.resize(data_size);
			file.read(&str[0], data_size);
			std::istringstream iss(str);
			for (int i = 0; i < loop; ++i)
			{
				iss.seekg(0);
				binary_iarchive ar(iss);
			//	test_serialize(ar, bdi);
				TimeCost& d = bdi;
#ifdef _DEBUG
			test_serialize(ar, d);
#else
			#include "test_body.h"
#endif
			}
			print_report("boost_bin_load", bdi, bdi);
		}
	}


template<class Archive>
void file_ar_run(const char* prefix, const char* mode, Archive*)
{
	FileStream file0("nark.bin", mode);
	file0.disbuf();

	TimeCost d;
	for (int i = 0; i < loop; ++i)
	{
		file0.seek(0);
		Archive ar;
		ar.attach(&file0);
	//	test_serialize(ar, d);
#ifdef _DEBUG
			test_serialize(ar, d);
#else
			#include "test_body.h"
#endif
	}
	print_report(prefix, d, Archive::is_loading::value ? bdi : bdo);
}

template<class Archive>
Archive new_mem_ar(unsigned char* buffer, int length, Archive*, MinMemIO*)
{
	(void)length;
	Archive ar; ar.set(buffer);
	return ar;
}
template<class Archive>
Archive new_mem_ar(unsigned char* buffer, int length, Archive*, MemIO*)
{
	Archive ar; ar.set(buffer, length);
	return ar;
}

template<class Archive>
void mem_ar_run(const char* prefix, unsigned char* buffer, int length, Archive*)
{
	TimeCost d;
//	unsigned char* tail = 0;
	for (int i = 0; i < loop; ++i)
	{
		Archive ar = new_mem_ar(buffer, length, (Archive*)0, (typename Archive::stream_t*)0);
#ifdef _DEBUG
			test_serialize(ar, d);
#else
			#include "test_body.h"
#endif
	//	tail = ar.current();

		if (ar.diff(buffer) > length)
		{
			fprintf(stderr, "buffer overrun\n");
			abort();
		}
	}
	print_report(prefix, d, Archive::is_loading::value ? bdi : bdo);
}

void run_nark()
{
	file_ar_run("File Save Native", "wb+", (NativeDataOutput<OutputBuffer>*)0);
 	file_ar_run("File Load Native", "rb" , (NativeDataInput<InputBuffer>*)0);
	file_ar_run("File Save Portable", "wb+", (PortableDataOutput<OutputBuffer>*)0);
	file_ar_run("File Load Portable", "rb" , (PortableDataInput<InputBuffer>*)0);

	file_ar_run("File Save NoVarInt Native", "wb+", (NativeNoVarIntOutput<OutputBuffer>*)0);
	file_ar_run("File Load NoVarInt Native", "rb" , (NativeNoVarIntInput<InputBuffer>*)0);
	file_ar_run("File Save NoVarInt Portable", "wb+", (PortableNoVarIntOutput<OutputBuffer>*)0);
	file_ar_run("File Load NoVarInt Portable", "rb" , (PortableNoVarIntInput<InputBuffer>*)0);

	int length = count*256;
	vector<unsigned char> vbuffer(length);
	unsigned char *buffer = &*vbuffer.begin();
	mem_ar_run("Memory Save Native", buffer, length, (NativeDataOutput<MemIO>*)(0));
	mem_ar_run("Memory Load Native", buffer, length, (NativeDataInput<MemIO>*)(0));
	mem_ar_run("Memory Save Portable", buffer, length, (PortableDataOutput<MemIO>*)(0));
	mem_ar_run("Memory Load Portable", buffer, length, (PortableDataInput<MemIO>*)(0));

	mem_ar_run("Memory Save NoVarInt Native", buffer, length, (NativeNoVarIntOutput<MemIO>*)(0));
	mem_ar_run("Memory Load NoVarInt Native", buffer, length, (NativeNoVarIntInput<MemIO>*)(0));
	mem_ar_run("Memory Save NoVarInt Portable", buffer, length, (PortableNoVarIntOutput<MemIO>*)(0));
	mem_ar_run("Memory Load NoVarInt Portable", buffer, length, (PortableNoVarIntInput<MemIO>*)(0));

	mem_ar_run("Uncheck Save Native", buffer, length, (NativeDataOutput<MinMemIO>*)(0));
	mem_ar_run("Uncheck Load Native", buffer, length, (NativeDataInput<MinMemIO>*)(0));
	mem_ar_run("Uncheck Save Portable", buffer, length, (PortableDataOutput<MinMemIO>*)(0));
	mem_ar_run("Uncheck Load Portable", buffer, length, (PortableDataInput<MinMemIO>*)(0));

	mem_ar_run("Uncheck Save NoVarInt Native", buffer, length, (NativeNoVarIntOutput<MinMemIO>*)(0));
	mem_ar_run("Uncheck Load NoVarInt Native", buffer, length, (NativeNoVarIntInput<MinMemIO>*)(0));
	mem_ar_run("Uncheck Save NoVarInt Portable", buffer, length, (PortableNoVarIntOutput<MinMemIO>*)(0));
	mem_ar_run("Uncheck Load NoVarInt Portable", buffer, length, (PortableNoVarIntInput<MinMemIO>*)(0));
}
};


int main(int argc, char* argv[])
{
	int loop = argc >= 2 ? atoi(argv[1]) : 1000;
	int count = argc >= 3 ? atoi(argv[2]) : 1000;

	printf("sizeof MyData1=%02d\n", (int)sizeof(MyData1));
	printf("sizeof MyData2=%02d\n", (int)sizeof(MyData2));
	printf("sizeof MyData3=%02d\n", (int)sizeof(MyData3));
	printf("sizeof MyData23=%02d\n", (int)sizeof(MyData23));
	printf("sizeof pair<int,int>=%02d\n", (int)sizeof(pair<int,int>));

	Bench ben(loop, count);
#ifdef _DEBUG
		ben.run_boost();
		ben.run_nark();
#else
	try {
		ben.run_boost();
		ben.run_nark();
	}
	catch (const std::exception& exp)
	{
		fprintf(stderr, "exception: %s\n", exp.what());
	}
#endif
	return 0;
}

