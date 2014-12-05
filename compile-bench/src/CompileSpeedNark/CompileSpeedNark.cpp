
#include <stdio.h>
#include <nark/io/DataIO.hpp>
#include <nark/io/StreamBuffer.hpp>
#include <nark/io/FileStream.hpp>

using namespace boost;
using namespace nark;
using namespace std;

typedef unsigned char uchar;
typedef signed char schar;

typedef long long longlong;
typedef unsigned long long ulonglong;

#include "data_nark.h"

int main(int argc, char* argv[])
{
	nark_pod_0 bp0;
	nark_pod_1 bp1;
	nark_pod_2 bp2;
	nark_complex_0 bc0;
	nark_complex_1 bc1;
	nark_complex_2 bc2;

	{
		FileStream file("boost.bin", "w+");
		NativeDataOutput<OutputBuffer> ar;
		ar.attach(&file);
		ar & bp0;
		ar & bp1;
		ar & bp2;
		ar & bc0;
		ar & bc1;
		ar & bc2;
		nark_foo(ar);
	}

	{
		FileStream file("boost.bin", "r");
		NativeDataInput<InputBuffer> ar;
		ar.attach(&file);
		ar & bp0;
		ar & bp1;
		ar & bp2;
		ar & bc0;
		ar & bc1;
		ar & bc2;
		nark_foo(ar);
	}
	return 0;
}

