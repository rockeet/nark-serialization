#include <autofree.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	nark::AutoFree<char> p((char*)malloc(100));
	p = (char*)malloc(10);
	strcpy(p.p, "abc");
	printf("%s\n", p.p);
	return 0;
}

