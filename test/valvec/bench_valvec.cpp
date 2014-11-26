#include <nark/valvec.hpp>
#include <nark/util/profiling.cpp>
#include <vector>

using namespace nark;

int main(int argc, char* argv[]) {
	nark::profiling pf;
	long M_cnt = argc >= 2 ? atoi(argv[1]) : 32;
	M_cnt *= 1024*1024;
	std::vector<int> v1(M_cnt);
	     valvec<int> v2(M_cnt);
	long t0 = pf.now();
	v1.push_back(1);
	long t1 = pf.now();
	v2.push_back(1);
	long t2 = pf.now();
	printf("msec: vector=%f valvec=%f vector/valvec=%f\n"
			, pf.mf(t0,t1)
			, pf.mf(t1,t2)
			, pf.mf(t0,t1)/pf.mf(t1,t2)
			);
	return 0;
}

