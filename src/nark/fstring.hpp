#ifndef __nark_fstring_hpp__
#define __nark_fstring_hpp__

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <iterator>
#include <string>
//#include <iosfwd>
#include <utility>

#include "config.hpp"
#include "stdtypes.hpp"
#include "util/throw.hpp"

#if defined(_MSC_VER)
	#include <boost/algorithm/searching/boyer_moore_horspool.hpp>
#endif
//#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>

namespace nark {

inline size_t nark_fstrlen(const char* s) { return strlen(s); }
inline size_t nark_fstrlen(const uint16_t* s) {
	size_t n = 0;
	while (s[n]) ++n;
	return n;
}

inline char*
nark_fstrstr(const char* haystack, size_t haystack_len
			 , const char* needle  , size_t needle_len)
{
#ifdef _MSC_VER
	const char* hay_end = haystack + haystack_len;
	const char* needle_end = needle + needle_len;
	const char* q = boost::algorithm::boyer_moore_horspool_search(
		haystack, hay_end, needle, needle_end);
	if (q == needle_end)
		return NULL;
	else
		return (char*)(q);
#else
	return (char*)memmem(haystack, haystack_len, needle, needle_len);
#endif
}

FEBIRD_DLL_EXPORT
uint16_t*
nark_fstrstr(const uint16_t* haystack, size_t haystack_len
			 , const uint16_t* needle  , size_t needle_len);

template<class Char>
struct nark_get_uchar_type;

template<>struct nark_get_uchar_type<char>{typedef unsigned char type;};
template<>struct nark_get_uchar_type<uint16_t>{typedef uint16_t type;};

// Fast String: shallow copy, simple, just has char* and length
// May be short name of: Febird String
template<class Char>
struct basic_fstring {
//	BOOST_STATIC_ASSERT(sizeof(Char) <= 2);
	typedef std::basic_string<Char> std_string;
	const Char* p;
	ptrdiff_t   n;

	basic_fstring() : p(NULL), n(0) {}
	basic_fstring(const std_string& x) : p(x.data()), n(x.size()) {}
#ifdef NDEBUG // let compiler compute strlen(string literal) at compilation time
	basic_fstring(const Char* x) : p(x), n(nark_fstrlen(x)) {}
#else
	basic_fstring(const Char* x) { assert(NULL != x); p = x; n = nark_fstrlen(x); }
#endif
	basic_fstring(const Char* x, ptrdiff_t   l) : p(x), n(l  ) { assert(l >= 0); }
	basic_fstring(const Char* x, const Char* y) : p(x), n(y-x) { assert(y >= x); }

#define fstring_enable_if_same_size(C) typename boost::enable_if_c<sizeof(C)==sizeof(Char)>::type* = NULL
	template<class C> basic_fstring(const C* x, fstring_enable_if_same_size(C)) { assert(NULL != x); p = x; n = nark_fstrlen((const Char*)x); }
	template<class C> basic_fstring(const C* x, ptrdiff_t l, fstring_enable_if_same_size(C)) : p((const Char*)x), n(l  ) { assert(l >= 0); }
	template<class C> basic_fstring(const C* x, const C*  y, fstring_enable_if_same_size(C)) : p((const Char*)x), n(y-x) { assert(y >= x); }
#undef fstring_enable_if_same_size

	basic_fstring(const std::pair<Char*, Char*>& rng) : p(rng.first), n(rng.second - rng.first) { assert(n >= 0); }
	basic_fstring(const std::pair<const Char*, const Char*>& rng) : p(rng.first), n(rng.second - rng.first) { assert(n >= 0); }

	template<class CharVec>
	basic_fstring(const CharVec& chvec, typename CharVec::const_iterator** =NULL) {
		p = &chvec[0];
		n = chvec.size();
	#ifndef NDEBUG
		if (chvec.size() > 1) {
		   	assert(&chvec[0]+1 == &chvec[1]);
		   	assert(&chvec[0]+n-1 == &chvec[n-1]);
	   	}
	#endif
	}

	const std::pair<const Char*, const Char*> range() const { return std::make_pair(p, p+n); }

	typedef ptrdiff_t difference_type;
	typedef    size_t       size_type;
	typedef const Char     value_type;
	typedef const Char &reference, &const_reference;
	typedef const Char *iterator, *const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator, const_reverse_iterator;
	typedef typename nark_get_uchar_type<Char>::type uc_t;

	iterator  begin() const { return p; }
	iterator cbegin() const { return p; }
	iterator    end() const { return p + n; }
	iterator   cend() const { return p + n; }
	reverse_iterator  rbegin() const { return reverse_iterator(p + n); }
	reverse_iterator crbegin() const { return reverse_iterator(p + n); }
	reverse_iterator    rend() const { return reverse_iterator(p); }
	reverse_iterator   crend() const { return reverse_iterator(p); }

	uc_t ende(ptrdiff_t off) const {
		assert(off <= n);
		assert(off >= 1);
		return p[n-off];
	}

	std_string    str() const { assert(0==n || (p && n) ); return std_string(p, n); }
	const Char* c_str() const { assert(p && '\0' == p[n]); return p; }
	const Char*  data() const { return p; }
	const uc_t* udata() const { return (const uc_t*)p; }
	size_t       size() const { return n; }
	int          ilen() const { return (int)n; } // for printf: "%.*s"
	uc_t operator[](ptrdiff_t i)const{assert(i>=0);assert(i<n);assert(p);return p[i];}
	uc_t        uch(ptrdiff_t i)const{assert(i>=0);assert(i<n);assert(p);return p[i];}

	bool empty() const { return 0 == n; }

	basic_fstring substr(size_t pos, size_t len) const {
		assert(pos <= size());
		if (pos > size()) { // similar with std::basic_string::substr
			THROW_STD(out_of_range, "size()=%zd pos=%zd", size(), pos);
		}
		if (len > size()) len = size(); // avoid pos+len overflow
		if (pos + len > size()) len = size() - pos;
	   	return basic_fstring(p+pos, p+pos+len);
   	}
	basic_fstring substr(size_t pos) const {
		assert(pos <= size());
		if (pos > size()) { // similar with std::basic_string::substr
			THROW_STD(out_of_range, "size()=%zd pos=%zd", size(), pos);
		}
	   	return basic_fstring(p+pos, p+n);
   	}

	bool match_at(ptrdiff_t pos, Char ch) const {
		assert(pos >= 0);
		assert(pos <= n);
		return pos < n && p[pos] == ch;
	}
	bool match_at(ptrdiff_t pos, basic_fstring needle) const {
		assert(pos >= 0);
		assert(pos <= n);
		if (pos + needle.n > n) return false;
		return memcmp(p + pos, needle.p, sizeof(Char) * needle.size()) == 0;
	}

	const Char* strstr(basic_fstring needle) const {
		assert(needle.n > 0);
		return this->strstr(0, needle);
	}
	const Char* strstr(ptrdiff_t pos, basic_fstring needle) const {
		assert(pos >= 0);
		assert(pos <= n);
		assert(needle.n > 0);
		if (pos + needle.n > n) return NULL;
		return nark_fstrstr(p, n, needle.p, needle.n);
	}

	bool begin_with(basic_fstring x) const {
		assert(x.n > 0);
		if (x.n > n) return false;
		return memcmp(p, x.p, sizeof(Char)*x.n) == 0;
	}
	bool end_with(basic_fstring x) const {
		assert(x.n > 0);
		if (x.n > n) return false;
		return memcmp(p+n - x.n, x.p, sizeof(Char)*x.n) == 0;
	}

	template<class Vec>
	size_t split(const Char delim, Vec* F, size_t max_fields = ~size_t(0)) const {
	//	assert(n >= 0);
		F->resize(0);
		if (' ' == delim) {
		   	// same as awk, skip first blank field, and skip dup blanks
			const Char *col = p, *End = p + n;
			while (col < End && isspace(*col)) ++col; // skip first blank field
			while (col < End && F->size()+1 < max_fields) {
				const Char* next = col;
				while (next < End && !isspace(*next)) ++next;
				F->push_back(typename Vec::value_type(col, next));
				while (next < End &&  isspace(*next)) ++next; // skip blanks
				col = next;
			}
			if (col < End)
				F->push_back(typename Vec::value_type(col, End));
		} else {
			const Char *col = p, *End = p + n;
			while (col <= End && F->size()+1 < max_fields) {
				const Char* next = col;
				while (next < End && delim != *next) ++next;
				F->push_back(typename Vec::value_type(col, next));
				col = next + 1;
			}
			if (col <= End)
				F->push_back(typename Vec::value_type(col, End));
		}
		return F->size();
	}

	/// split into fields
	template<class Vec>
	size_t split(const Char* delims, Vec* F, size_t max_fields = ~size_t(0)) {
		assert(n >= 0);
		size_t dlen = nark_fstrlen(delims);
		if (0 == dlen) // empty delims redirect to blank delim
			return split(' ', F);
		if (1 == dlen)
			return split(delims[0], F);
		F->resize(0);
		const Char *col = p, *End = p + n;
		while (col <= End && F->size()+1 < max_fields) {
			const Char* next = nark_fstrstr(col, End-col, delims, dlen);
			if (NULL == next) next = End;
			F->push_back(typename Vec::value_type(col, next));
			col = next + dlen;
		}
		if (col <= End)
			F->push_back(typename Vec::value_type(col, End));
		return F->size();
	}
};

template<class DataIO, class Char>
void DataIO_saveObject(DataIO& dio, basic_fstring<Char> s) {
	dio << typename DataIO::my_var_uint64_t(s.n);
	dio.ensureWrite(s.p, sizeof(Char) * s.n);
}

typedef basic_fstring<char> fstring;
typedef basic_fstring<uint16_t> fstring16;

template<class Char> struct char_to_fstring;
template<> struct char_to_fstring<char> { typedef fstring type; };
template<> struct char_to_fstring<unsigned char> { typedef fstring type; };
template<> struct char_to_fstring<uint16_t> { typedef fstring16 type; };

FEBIRD_DLL_EXPORT
std::string operator+(fstring x, fstring y);
inline std::string operator+(fstring x, const char* y) {return x+fstring(y);}
inline std::string operator+(const char* x, fstring y) {return fstring(x)+y;}
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
inline std::string operator+(std::string&& x, fstring y) { return x.append(   y.p, y.n); }
inline std::string operator+(fstring x, std::string&& y) { return y.insert(0, x.p, x.n); }
#endif

FEBIRD_DLL_EXPORT bool operator==(fstring x, fstring y);
FEBIRD_DLL_EXPORT bool operator!=(fstring x, fstring y);

FEBIRD_DLL_EXPORT bool operator<(fstring x, fstring y);
FEBIRD_DLL_EXPORT bool operator>(fstring x, fstring y);

FEBIRD_DLL_EXPORT bool operator<=(fstring x, fstring y);
FEBIRD_DLL_EXPORT bool operator>=(fstring x, fstring y);

/*
// ostream is brain dead binary incompatible, use inline
inline std::ostream& operator<<(std::ostream& os, fstring s) {
	os.write(s.p, s.n);
	return os;
}
*/

// fstring16
FEBIRD_DLL_EXPORT bool operator==(fstring16 x, fstring16 y);
FEBIRD_DLL_EXPORT bool operator!=(fstring16 x, fstring16 y);

FEBIRD_DLL_EXPORT bool operator<(fstring16 x, fstring16 y);
FEBIRD_DLL_EXPORT bool operator>(fstring16 x, fstring16 y);

FEBIRD_DLL_EXPORT bool operator<=(fstring16 x, fstring16 y);
FEBIRD_DLL_EXPORT bool operator>=(fstring16 x, fstring16 y);

} // namespace nark

#endif // __nark_fstring_hpp__
