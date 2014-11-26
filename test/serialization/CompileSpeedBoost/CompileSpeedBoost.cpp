// CompileSpeedBoost.cpp : Defines the entry point for the console application.
//

#include <fstream>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/deque.hpp>

#define DATA_IO_LOAD_SAVE(Class, Members) \
    friend class boost::serialization::access; \
    template<class Archive> \
    void serialize(Archive & ar, const unsigned int version) \
	{ ar Members; }

using namespace boost;
using namespace std;

typedef unsigned char uchar;
typedef signed char schar;

typedef unsigned short ushort;
typedef unsigned long ulong;

typedef long long longlong;
typedef unsigned long long ulonglong;


#include "data_boost.h"

using namespace boost::archive;


int main(int argc, char* argv[])
{
	boost_pod_0 bp0;
	boost_pod_1 bp1;
	boost_pod_2 bp2;
	boost_complex_0 bc0;
	boost_complex_1 bc1;
	boost_complex_2 bc2;

	{
		std::ofstream file("boost.bin", ios::binary);
		binary_oarchive ar(file);
		ar & bp0;
		ar & bp1;
		ar & bp2;
		ar & bc0;
		ar & bc1;
		ar & bc2;
		boost_foo(ar);
	}
	{
		std::ifstream file("boost.bin", ios::binary);
		binary_iarchive ar(file);
		ar & bp0;
		ar & bp1;
		ar & bp2;
		ar & bc0;
		ar & bc1;
		ar & bc2;
		boost_foo(ar);
	}
	return 0;
}

