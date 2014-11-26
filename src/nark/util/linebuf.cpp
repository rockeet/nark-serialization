#include "linebuf.hpp"
#include "throw.hpp"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdexcept>

namespace nark {

LineBuf::LineBuf()
   	: capacity(0), n(0), p(NULL)
{}

LineBuf::~LineBuf() {
   	if (p)
	   	free(p);
}

ptrdiff_t LineBuf::getline(FILE* f) {
	assert(NULL != f);
#if defined(__USE_GNU) || defined(__CYGWIN__) || defined(__CYGWIN32__)
       	// has ::getline
	return n = ::getline(&p, &capacity, f);
#else
//	#error only _GNU_SOURCE is supported
	if (NULL == p) {
		capacity = BUFSIZ;
		p = (char*)malloc(BUFSIZ);
	}
	char* ret = ::fgets(p, capacity, f);
	if (ret) {
		size_t len = ::strlen(p);
		while (len == capacity-1 && p[len-1] == '\n') {
			ret = (char*)realloc(p, capacity*2);
			if (NULL == ret) {
				throw std::bad_alloc();
			}
			p = ret;
			ret = ::fgets(p + len, capacity+1, f);
			len = len + ::strlen(ret);
			capacity *= 2;
		}
		return n = ptrdiff_t(len);
	} else {
		return -1;
	}
#endif
}

size_t LineBuf::trim() {
	assert(NULL != p);
	size_t n0 = n;
	while (n > 0 && isspace(p[n-1])) p[--n] = 0;
	return n0 - n;
}

size_t LineBuf::chomp() {
	assert(NULL != p);
	size_t n0 = n;
	while (n > 0 && strchr("\r\n", p[n-1])) p[--n] = 0;
	return n0 - n;
}

bool LineBuf::read_binary_tuple(int32_t* offsets, size_t arity, FILE* f) {
	assert(NULL != offsets);
	offsets[0] = 0;
	size_t n_read = fread(offsets+1, 1, sizeof(int32_t) * arity, f);
	if (n_read != sizeof(int32_t) * arity) {
		return false;
	}
	for (size_t i = 0; i < arity; ++i) {
		assert(offsets[i+1] >= 0);
		offsets[i+1] += offsets[i];
	}
	size_t len = offsets[arity];
	if (this->capacity < len) {
		char* q = (char*)realloc(this->p, len);
		if (NULL == q) {
			THROW_STD(invalid_argument
				, "Out of memory when reading record[size=%zd(0x%zX)]"
				, len, len
				);
		}
		this->p = q;
		this->capacity = len;
	}
	n_read = fread(this->p, 1, len, f);
	if (n_read != len) {
		THROW_STD(invalid_argument
			, "fread record data failed: request=%zd, readed=%zd\n"
			, len, n_read
			);
	}
	this->n = len;
	return true; // len can be 0
}

} // namespace nark

