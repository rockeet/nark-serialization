#define FEBIRD_DATA_IO_SLOW_VAR_INT

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <nark/io/MemStream.hpp>
#include <nark/io/DataIO.hpp>
#include <nark/util/profiling.hpp>

using namespace std;
using namespace nark;

template<class Stream>
void fast_read_v32(int size, int loop, Stream s)
{
	for (int j = 0; j < loop; ++j) {
		Stream input = s;
		for (int i = 0; i < size; ++i) {
			volatile uint32_t x = input.read_var_uint32();
		}
		input = s;
		for (int i = 0; i < size; ++i) {
			volatile uint32_t x = input.read_var_uint32();
		}
	}
}

template<class Stream>
void slow_read_v32(int size, int loop, Stream s)
{
	for (int j = 0; j < loop; ++j) {
		NativeDataInput<Stream> input;
		input.set(s);
		// 重复两次以便 '>>' 不会被 inline
		for (int i = 0; i < size; ++i) {
			var_uint32_t x;
			input >> x;
			volatile var_uint32_t y = x;
		}
		input.set(s);
		for (int i = 0; i < size; ++i) {
			var_uint32_t x;
			input >> x;
			volatile var_uint32_t y = x;
		}
	}
}

template<class Stream, class VarUint>
void read_v30(int size, int loop, Stream s, VarUint* x)
{
	for (int j = 0; j < loop; ++j) {
		NativeDataInput<Stream> input;
		input.set(s);
		for (int i = 0; i < size; ++i) {
			input >> *x;
		}
		input.set(s);
		for (int i = 0; i < size; ++i) {
			input >> *x;
		}
	}
}

template<class Uint, class VarUint>
void test(int size, int loop, Uint*, VarUint*)
{

}

int main(int argc, char* argv[])
{
    int size = 256;
    int loop = 256;
    if (argc > 1)
        size = atoi(argv[1]);
    if (argc > 2)
        loop = atoi(argv[2]);

    vector<uint32_t> vec(size);
    for (int i = 0; i < size; ++i)
        vec[i] = rand() % (1<<30);

    NativeDataOutput<AutoGrownMemIO> fast;
    NativeDataOutput<AutoGrownMemIO> slow;
	NativeDataOutput<AutoGrownMemIO> uv30;
    fast.resize(size * 5);
    slow.resize(size * 5);
	uv30.resize(size * 5);

    profiling pf;
    long long t0 = pf.now(); // start fast write var_uint32
    for (int j = 0; j < loop; ++j) {
        fast.rewind();
        for (int i = 0; i < size; ++i) {
            fast.write_var_uint32(vec[i]);
        }
    }
    long long t1 = pf.now(); // start slow write var_uint32
    for (int j = 0; j < loop; ++j) {
        slow.rewind();
        for (int i = 0; i < size; ++i) {
            slow << var_uint32_t(vec[i]);
        }
    }
    long long t2 = pf.now(); // start fast read var_uint32

	fast_read_v32(size, loop, fast.head());

    long long t3 = pf.now(); // start slow read

	slow_read_v32(size, loop, slow.head());

    long long t4 = pf.now(); // start write var_uint30
	for (int j = 0; j < loop; ++j) {
		uv30.rewind();
		for (int i = 0; i < size; ++i) {
			uv30.write_var_uint30(vec[i]);
		}
	}
	long long t5 = pf.now(); // start read var_uint30
	var_uint30_t xxxxx;
	read_v30(size, loop, uv30.head(), &xxxxx);
    long long t6 = pf.now();

    if (fast.tell() != slow.tell() || memcmp(fast.begin(), slow.begin(), fast.tell()) != 0) {
        printf("error: fast != slow\n");
    }

    // check correction
    var_uint32_t v1(12345678);
    slow.rewind();  slow << v1;
    slow.rewind();
    uint32_t v2 = slow.read_var_uint32();
    assert(v1.t == v2);

    slow.rewind();
    slow.write_var_uint32(v2);
	{
		NativeDataInput<MemIO> input;
		input.set(slow.head());
		input >> v1;
	}
    assert(v1.t == v2);

    printf("save[fast=%lld'us, slow=%lld'us, slow/fast=%f, avg=%f'ns, speed=%f'MB/S]\n"
        , pf.us(t0,t1), pf.us(t1,t2)
        , (double)(t2-t1)/(t1-t0)
        , (double)pf.ns(t0,t1)/(loop * size)
		, (double)(sizeof(uint32_t) * loop * size) * 1e3 / pf.ns(t0,t1)
        );
    printf("load[fast=%lld'us, slow=%lld'us, slow/fast=%f, avg=%f'ns, speed=%f'MB/S]\n"
        , pf.us(t2,t3), pf.us(t3,t4)
        , (double)(t4-t3)/(t3-t2)
        , (double)pf.ns(t2,t3)/(loop * size * 2)
		, (double)(sizeof(uint32_t) * loop * size) * 1e3 / pf.ns(t2,t3)
        );
	printf("save[vu30=%lld'us, fast=%lld'us, fast/vu30=%f, avg=%f'ns, speed=%f'MB/S]\n"
		, pf.us(t4,t5), pf.us(t0,t1)
		, (double)(t1-t0)/(t5-t4)
		, (double)pf.ns(t4,t5)/(loop * size)
		, (double)(sizeof(uint32_t) * loop * size) * 1e3 / pf.ns(t4,t5)
		);
	printf("load[vu30=%lld'us, fast=%lld'us, fast/vu30=%f, avg=%f'ns, speed=%f'MB/S]\n"
		, pf.us(t5,t6), pf.us(t2,t3)
		, (double)(t3-t2)/(t6-t5)
		, (double)pf.ns(t5,t6)/(loop * size * 2)
		, (double)(sizeof(uint32_t) * loop * size) * 1e3 / pf.ns(t5,t6)
		);

    return 0;
}
