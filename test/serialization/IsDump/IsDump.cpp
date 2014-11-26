#include "stdafx.h"
#include <nark/io/DataIO.hpp>
#include <nark/io/MemStream.hpp>
//#include <nark/io/DataIO_Dump.hpp>

#include <boost/type_traits/detail/bool_trait_def.hpp>

//#include <boost/function.hpp>

struct Dumpable
{
	int x, y, z;
};
DATA_IO_DUMP_RAW_MEM_E(Dumpable)

struct Dumpable2
{
	unsigned char a, b, c, d;
	DATA_IO_LOAD_SAVE(Dumpable2, &a &b &c &d)
};
//Gen_DataIO_IsDump_True(Dumpable2)
// namespace nark {
// DataIO_IsDump_TypeTrue1(Dumpable2)
// }

template<class Dio, class T>
nark::DataIO_is_dump<Dio, T>
DataIO_IsDump(Dio*, T*)
{
	return nark::DataIO_is_dump<Dio, T>();
}

int main(int argc, char* argv[])
{
	using namespace nark;
	using namespace std;

	uint8_t  u8;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
	int8_t  s8;
	int16_t s16;
	int32_t s32;
	int64_t s64;
	char  c;

	NativeDataInput<MemIO> iNative;
	PortableDataInput<MemIO> iPortable;
	NativeDataOutput<AutoGrownMemIO> oNative;
	PortableDataOutput<AutoGrownMemIO> oPortable;
//	printf("DataIO_IsDump(NativeDataInput,uint32_t)=%d\n", DataIO_IsDump(&iNative, &u32).value);

	assert(DataIO_IsDump(&iNative, &c ).value);
	assert(DataIO_IsDump(&iNative, &u8 ).value);
	assert(DataIO_IsDump(&iNative, &u16).value);
	assert(DataIO_IsDump(&iNative, &u32).value);
	assert(DataIO_IsDump(&iNative, &u64).value);
	assert(DataIO_IsDump(&iNative, &u32).value);

	assert(DataIO_IsDump(&oNative, &c ).value);
	assert(DataIO_IsDump(&oNative, &u8 ).value);
	assert(DataIO_IsDump(&oNative, &u16).value);
	assert(DataIO_IsDump(&oNative, &u32).value);
	assert(DataIO_IsDump(&oNative, &u64).value);
	assert(DataIO_IsDump(&oNative, &u32).value);

	assert( DataIO_IsDump(&iPortable, &c ).value);
	assert( DataIO_IsDump(&iPortable, &u8 ).value);
	assert(!DataIO_IsDump(&iPortable, &u16).value);
	assert(!DataIO_IsDump(&iPortable, &u32).value);
	assert(!DataIO_IsDump(&iPortable, &u64).value);
	assert(!DataIO_IsDump(&iPortable, &u32).value);

	assert( DataIO_IsDump(&oPortable, &c ).value);
	assert( DataIO_IsDump(&oPortable, &u8 ).value);
	assert(!DataIO_IsDump(&oPortable, &u16).value);
	assert(!DataIO_IsDump(&oPortable, &u32).value);
	assert(!DataIO_IsDump(&oPortable, &u64).value);
	assert(!DataIO_IsDump(&oPortable, &u32).value);

	assert(DataIO_IsDump(&iNative, &s8 ).value);
	assert(DataIO_IsDump(&iNative, &s16).value);
	assert(DataIO_IsDump(&iNative, &s32).value);
	assert(DataIO_IsDump(&iNative, &s64).value);
	assert(DataIO_IsDump(&iNative, &s32).value);

	assert(DataIO_IsDump(&oNative, &s8 ).value);
	assert(DataIO_IsDump(&oNative, &s16).value);
	assert(DataIO_IsDump(&oNative, &s32).value);
	assert(DataIO_IsDump(&oNative, &s64).value);
	assert(DataIO_IsDump(&oNative, &s32).value);

	assert( DataIO_IsDump(&iPortable, &s8 ).value);
	assert(!DataIO_IsDump(&iPortable, &s16).value);
	assert(!DataIO_IsDump(&iPortable, &s32).value);
	assert(!DataIO_IsDump(&iPortable, &s64).value);
	assert(!DataIO_IsDump(&iPortable, &s32).value);

	assert( DataIO_IsDump(&oPortable, &s8 ).value);
	assert(!DataIO_IsDump(&oPortable, &s16).value);
	assert(!DataIO_IsDump(&oPortable, &s32).value);
	assert(!DataIO_IsDump(&oPortable, &s64).value);
	assert(!DataIO_IsDump(&oPortable, &s32).value);

	std::pair<uint16_t, uint16_t> p16_16;
	std::pair<uint16_t, uint32_t> p16_32; // has padding, so can not dump
	std::pair<uint32_t, uint32_t> p32_32;
	std::pair<uint64_t, uint64_t> p64_64;
	assert( DataIO_IsDump(&oNative, &p16_16).value);
	assert(!DataIO_IsDump(&oNative, &p16_32).value);
	assert( DataIO_IsDump(&oNative, &p32_32).value);
	assert( DataIO_IsDump(&oNative, &p64_64).value);

	Dumpable d;
	assert( DataIO_IsDump(&oNative, &d).value);

	Dumpable2 d2;
	assert( DataIO_IsDump(&oNative, &d2).value);

	vector<uint32_t> v_u32;
	for (int i = 0; i < 100; ++i)
		v_u32.push_back(i+1);

	oNative << v_u32;

	printf("OK!\n");

	return 0;
}

