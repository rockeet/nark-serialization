// GenCompileBench.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void gen_pod_serialize(FILE* fp, const char* prefix, const vector<string>& types)
{
	for (int i = 0; i != types.size(); ++i)
	{
		fprintf(fp, " & a%d", i);
	}
}

void gen_pod(FILE* fp, const char* prefix, int no, const vector<string>& types)
{
	fprintf(fp, "struct %s_pod_%d\n", prefix, no);
	fprintf(fp, "{\n");
	for (int i = 0; i != types.size(); ++i)
	{
		fprintf(fp, "   %s a%d;\n", types[i].c_str(), i);
	}
	fprintf(fp, "\n");
	fprintf(fp, "    DATA_IO_LOAD_SAVE(%s_pod_%d, ", prefix, no);
	gen_pod_serialize(fp, prefix, types);
	fprintf(fp, ")\n");
	fprintf(fp, "};\n");
}

void gen_complex_serialize(FILE* fp, const char* prefix, const vector<string>& types)
{
	fprintf(fp, "\n");
	for (int i = 0; i != types.size(); ++i)
	{
		fprintf(fp, "       & map_%s_%d\n", types[i].c_str(), i);
		fprintf(fp, "       & multimap_%s_%d\n", types[i].c_str(), i);
		fprintf(fp, "       & set_%s_%d\n", types[i].c_str(), i);
		fprintf(fp, "       & multiset_%s_%d\n", types[i].c_str(), i);
		fprintf(fp, "       & vector_%s_%d\n", types[i].c_str(), i);
		fprintf(fp, "       & deque_%s_%d\n", types[i].c_str(), i);
		fprintf(fp, "       & list_%s_%d\n", types[i].c_str(), i);
	}
}

void gen_complex(FILE* fp, const char* prefix, int no, const vector<string>& types)
{
	fprintf(fp, "struct %s_complex_%d\n", prefix, no);
	fprintf(fp, "{\n");
	for (int i = 0; i != types.size(); ++i)
	{
		fprintf(fp, "   std::map<int, %s > map_%s_%d;\n", types[i].c_str(), types[i].c_str(), i);
		fprintf(fp, "   std::multimap<int, %s > multimap_%s_%d;\n", types[i].c_str(), types[i].c_str(), i);
		fprintf(fp, "   std::set<%s > set_%s_%d;\n", types[i].c_str(), types[i].c_str(), i);
		fprintf(fp, "   std::multiset<%s > multiset_%s_%d;\n", types[i].c_str(), types[i].c_str(), i);
		fprintf(fp, "   std::vector<%s > vector_%s_%d;\n", types[i].c_str(), types[i].c_str(), i);
		fprintf(fp, "   std::deque<%s > deque_%s_%d;\n", types[i].c_str(), types[i].c_str(), i);
		fprintf(fp, "   std::list<%s > list_%s_%d;\n", types[i].c_str(), types[i].c_str(), i);
	}
	fprintf(fp, "\n");

	fprintf(fp, "    DATA_IO_LOAD_SAVE(%s_complex_%d, ", prefix, no);
	gen_complex_serialize(fp, prefix, types);
	fprintf(fp, ")\n");

	fprintf(fp, "};\n");
}

void gen(FILE* fp, int count, const char* prefix, const vector<string>& types)
{
	vector<string> types2 = types;
	gen_pod(fp, prefix, 0, types);
	gen_complex(fp, prefix, 0, types);

	for (int i = 1; i < count; ++i)
	{
		std::random_shuffle(types2.begin(), types2.end());
		gen_pod(fp, prefix, i, types2);
		gen_complex(fp, prefix, i, types2);
	}

	fprintf(fp, "template<class Archive> void %s_foo(Archive& ar)\n", prefix);
	fprintf(fp, "{\n");
	for (int i = 0; i < count; ++i)
	{
		fprintf(fp, "    %s_pod_%d a_%s_pod_%d;\n", prefix, i, prefix, i);
		fprintf(fp, "    %s_complex_%d a_%s_complex_%d;\n", prefix, i, prefix, i);
	}
	for (int i = 0; i < count; ++i)
	{
		fprintf(fp, "    ar & a_%s_pod_%d;\n", prefix, i);
		fprintf(fp, "    ar & a_%s_complex_%d;\n", prefix, i);
	}
	fprintf(fp, "}\n");
}

int main(int argc, char* argv[])
{
	int count = argc >= 2 ? atoi(argv[1]) : 20;
	const char* fname = "data_nark.h";
	FILE* fp = fopen(fname, "w+");
	if (0 == fp)
	{
		fprintf(stderr, "open %s error\n", fname);
		exit(-1);
	}
	vector<string> types;
	types.push_back("char");
	types.push_back("uchar");
	types.push_back("schar");
	types.push_back("schar");

	types.push_back("short");
	types.push_back("ushort");

	types.push_back("wchar_t");
	types.push_back("wchar_t");
	types.push_back("float");

	types.push_back("double");
	types.push_back("int");
	types.push_back("unsigned");
	types.push_back("long");
	types.push_back("ulong");
	types.push_back("longlong");
	types.push_back("ulonglong");

	gen(fp, count, "nark", types);

	fclose(fp);

	fname = "data_boost.h";
	fp = fopen(fname, "w+");
	if (0 == fp)
	{
		fprintf(stderr, "open %s error\n", fname);
		exit(-1);
	}
	gen(fp, count, "boost", types);

	fclose(fp);

	printf("ok!\n");

	return 0;
}

