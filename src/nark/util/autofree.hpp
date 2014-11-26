#ifndef __nark_util_autofree_hpp__
#define __nark_util_autofree_hpp__

#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <stdlib.h>
#include <stdexcept>

namespace nark {

	template<class T>
	class AutoFree : boost::noncopyable {
		BOOST_STATIC_ASSERT(boost::has_trivial_destructor<T>::value);

	public:
		T* p; // just the pointer, no any overhead

		AutoFree() : p(NULL) {}
		explicit AutoFree(T* q) : p(q) {}
		explicit AutoFree(size_t n) {
			p = (T*)::malloc(sizeof(T) * n);
			if (NULL == p)
				throw std::bad_alloc();
		}
		AutoFree(size_t n, T val) {
			p = (T*)::malloc(sizeof(T) * n);
			if (NULL == p)
				throw std::bad_alloc();
			register T* r = p;
			for (size_t i = 0; i < n; ++i) r[i] = val;
		}
		~AutoFree() { if (p) ::free(p); }

		void operator=(T* q) {
		   	if (p)
			   	::free(p);
		   	p = q;
	   	}

		void free() {
		   	assert(p);
		   	::free(p);
		   	p = NULL;
	   	}

		void alloc(size_t n) {
			if (p) ::free(p);
			p = (T*)::malloc(sizeof(T) * n);
			if (NULL == p)
				throw std::bad_alloc();
		}

		void resize(size_t n) {
			T* q = (T*)::realloc(p, sizeof(T) * n);
			if (NULL == q)
				throw std::bad_alloc();
			p = q;
		}

		void swap(AutoFree& y) { T* tmp = p; p = y.p; y.p = tmp; }

		operator T*  () const { return  p; }
		T* operator->() const { return  p; } // ? direct, simple and stupid ?
		T& operator* () const { return *p; } // ? direct, simple and stupid ?
	//	T& operator[](int i) const { return p[i]; }
	};

} // namespace nark

namespace std {
	template<class T>
	void swap(nark::AutoFree<T>& x, nark::AutoFree<T>& y) { x.swap(y); }
}

#endif


