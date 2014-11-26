#include <nark/util/linebuf.cpp>
#include <nark/valvec.hpp>
#include <getopt.h>

int main(int argc, char* argv[]) {
	const char* delims = " ";
	for (;;) {
		int opt = getopt(argc, argv, "F:");
		switch (opt) {
			default:
				goto GetoptDone;
			case 'F':
				delims = optarg;
				break;
		}
	}
GetoptDone:
	printf("delims=%s:\n", delims);
	nark::LineBuf lb;
	nark::valvec<std::string> F;
	while (lb.getline(stdin) >= 0) {
		lb.chomp();
		lb.split_by_all(delims, &F);
		for (size_t i = 0; i < F.size(); ++i)
			printf(":%s:\t", F[i].c_str());
		printf("$\n");
	}
	return 0;
}

