#include "fstring.hpp"
#include "hash_strmap.hpp"
//#include <ostream>

namespace nark {

std::string operator+(fstring x, fstring y) {
	std::string z;
	z.reserve(x.n + y.n);
	z.append(x.p, x.n);
	z.append(y.p, y.n);
	return z;
}

bool operator==(fstring x, fstring y) { return  fstring_func::equal()(x, y); }
bool operator!=(fstring x, fstring y) { return !fstring_func::equal()(x, y); }

bool operator<(fstring x, fstring y) {
	ptrdiff_t n = std::min(x.n, y.n);
	int ret = memcmp(x.p, y.p, n);
	if (ret)
		return ret < 0;
	else
		return x.n < y.n;
}
bool operator> (fstring x, fstring y) { return   y < x ; }
bool operator<=(fstring x, fstring y) { return !(y < x); }
bool operator>=(fstring x, fstring y) { return !(x < y); }

// fstring16

bool operator==(fstring16 x, fstring16 y) {
	if (x.n != y.n) return false;
	return memcmp(x.p, y.p, 2*x.n) == 0;
}
bool operator!=(fstring16 x, fstring16 y) { return !(x == y); }

bool operator<(fstring16 x, fstring16 y) {
	ptrdiff_t n = std::min(x.n, y.n);
	int ret = 0;
	for (ptrdiff_t i = 0; i < n; ++i) {
		uint16_t cx = x.p[i];
		uint16_t cy = y.p[i];
		if (cx != cy) {
			if (sizeof(int) > 2)
				ret = cx - cy;
			else if (cx < cy)
				ret = -1;
			else
				ret = 1;
			break;
		}
	}
	if (ret)
		return ret < 0;
	else
		return x.n < y.n;
}
bool operator> (fstring16 x, fstring16 y) { return   y < x ; }
bool operator<=(fstring16 x, fstring16 y) { return !(y < x); }
bool operator>=(fstring16 x, fstring16 y) { return !(x < y); }

uint16_t*
nark_fstrstr(const uint16_t* haystack, size_t haystack_len
			 , const uint16_t* needle  , size_t needle_len)
{
#if defined(_MSC_VER)
	const uint16_t* hay_end = haystack + haystack_len;
	const uint16_t* needle_end = needle + needle_len;
	const uint16_t* q = boost::algorithm::boyer_moore_horspool_search(
		haystack, hay_end, needle, needle_end);
	if (q == needle_end)
		return NULL;
	else
		return (uint16_t*)(q);
#else
	char*  hit = (char*)(haystack);
	char*  end = (char*)(haystack + haystack_len);
	ptrdiff_t needle_len2 = needle_len * 2;
	while (end - hit  >= needle_len2) {
		hit = (char*)memmem(hit, end - hit, needle, needle_len2);
		if (NULL == hit || (hit - (char*)haystack) % 2 == 0)
			return (uint16_t*)hit;
		hit++;
	}
#endif
	return NULL;
}

} // namespace nark
