#ifndef __penglei_valvec_hpp__
#define __penglei_valvec_hpp__

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <functional>
#include <utility>

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

//
#if defined(__GNUC__) && __GNUC__ >= 4 && !defined(__GXX_EXPERIMENTAL_CXX0X__)
	#include <ext/memory> // for uninitialized_copy_n
	#include <ext/algorithm> // for copy_n
	#define STDEXT_copy_n               __gnu_cxx::copy_n
	#define STDEXT_uninitialized_copy_n __gnu_cxx::uninitialized_copy_n
#else
	#define STDEXT_copy_n               std::copy_n
	#define STDEXT_uninitialized_copy_n std::uninitialized_copy_n
#endif

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L || \
	defined(_MSC_VER) && _MSC_VER >= 1700
	#include <initializer_list>
	#ifndef HSM_HAS_MOVE
		#define HSM_HAS_MOVE
	#endif
#endif

#ifndef __HAS_RVALUE_REFERENCE
  #if (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 3) || \
								       _MSC_VER >= 1600
    #if __EDG_VERSION__ > 0
      #define __HAS_RVALUE_REFERENCE (__EDG_VERSION__ >= 410)
    #else
      #define __HAS_RVALUE_REFERENCE 1
    #endif
  #elif __clang__
    #define __HAS_RVALUE_REFERENCE __has_feature(cxx_rvalue_references)
  #else
    #define __HAS_RVALUE_REFERENCE 0
  #endif
#endif

#ifdef _MSC_VER
	template<class T>
	void STDEXT_destroy_range_aux(T*, T*, boost::mpl::true_) {}
	template<class T>
	void STDEXT_destroy_range_aux(T* p, T* q, boost::mpl::false_) {
		for (; p < q; ++p) p->~T();
	}
	template<class T>
	void STDEXT_destroy_range(T* p, T* q) {
		STDEXT_destroy_range_aux(p, q, boost::has_trivial_destructor<T>());
	}
#else
	#define STDEXT_destroy_range std::_Destroy
#endif

template<class T>
inline bool is_object_overlap(const T* x, const T* y) {
	assert(NULL != x);
	assert(NULL != y);
	if (x+1 <= y || y+1 <= x)
		return false;
	else
		return true;
}

struct valvec_no_init {
	template<class Vec>
	void operator()(Vec& v, size_t n) const { v.resize_no_init(n); }
};
struct valvec_reserve {
	template<class Vec>
	void operator()(Vec& v, size_t n) const { v.reserve(n); }
};

/// similary with std::vector, but:
///   1. use realloc to enlarge/shrink memory, this has avoid memcpy when
///      realloc is implemented by mremap for large chunk of memory;
///      mainstream realloc are implemented in this way
///   2. valvec also avoid calling copy-cons when enlarge the valvec
///@Note
///  1. T must be memmove-able, std::list,map,set... are not memmove-able
///  2. std::vector, string, ... are memmove-able, they could be the T
template<class T>
class valvec {
protected:
	struct AutoMemory { // for exception-safe
		T* p;
		explicit AutoMemory(size_t n) {
			p = (T*)malloc(sizeof(T) * n);
			if (NULL == p) throw std::bad_alloc();
		}
	//	explicit AutoMemory(T* q) {
	//		p = q;
	//		if (NULL == q) throw std::bad_alloc();
	//	}
	//	AutoMemory() { p = NULL; }
		~AutoMemory() { if (p) free(p); }
	};

    T*     p;
    size_t n;
    size_t c; // capacity

	template<class InputIter>
	void construct(InputIter first, ptrdiff_t count) {
		assert(count >= 0);
		p = NULL;
		n = c = 0;
		if (count) { // for exception-safe
			AutoMemory tmp(count);
			STDEXT_uninitialized_copy_n(first, count, tmp.p);
			p = tmp.p;
			n = c = count;
			tmp.p = NULL;
		}
	}

	template<class> struct void_ { typedef void type; };

	template<class InputIter>
	void construct(InputIter first, InputIter last, std::input_iterator_tag) {
		p = NULL;
		n = c = 0;
		for (; first != last; ++first)
			this->push_back(*first);
	}
	template<class ForwardIter>
	void construct(ForwardIter first, ForwardIter last, std::forward_iterator_tag) {
		ptrdiff_t count = std::distance(first, last);
		assert(count >= 0);
		construct(first, count);
	}

	template<class InputIter>
	void assign_aux(InputIter first, InputIter last, std::input_iterator_tag) {
		resize(0);
		for (; first != last; ++first)
			this->push_back(*first);
	}
	template<class ForwardIter>
	void assign_aux(ForwardIter first, ForwardIter last, std::forward_iterator_tag) {
		ptrdiff_t count = std::distance(first, last);
		assign(first, count);
	}

public:
    typedef T  value_type;
    typedef T* iterator;
    typedef T& reference;
    typedef const T* const_iterator;
    typedef const T& const_reference;

    typedef std::reverse_iterator<T*> reverse_iterator;
    typedef std::reverse_iterator<const T*> const_reverse_iterator;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    T* begin() { return p; }
    T* end()   { return p + n; }

    const T* begin() const { return p; }
    const T* end()   const { return p + n; }

    T* rbegin() { return p + n; }
    T* rend()   { return p; }

    const T* rbegin() const { return p + n; }
    const T* rend()   const { return p; }

    const T* cbegin() const { return p; }
    const T* cend()   const { return p + n; }

    const T* crbegin() const { return p + n; }
    const T* crend()   const { return p; }

    valvec() {
        p = NULL;
        n = c = 0;
    }

    explicit valvec(size_t sz, const T& val = T()) {
        if (sz) {
			AutoMemory tmp(sz);
            std::uninitialized_fill_n(tmp.p, sz, val);
			p = tmp.p;
			n = c = sz;
			tmp.p = NULL;
        } else {
            p = NULL;
            n = c = 0;
        }
    }

	// size = capacity = sz, memory is not initialized/constructed
	valvec(size_t sz, valvec_no_init) {
		p = NULL;
		n = c = 0;
		resize_no_init(sz);
	}
	// size = 0, capacity = sz
	valvec(size_t sz, valvec_reserve) {
		p = NULL;
		n = c = 0;
		reserve(sz);
	}

	template<class AnyIter>
	valvec(const std::pair<AnyIter, AnyIter>& rng
		, typename AnyIter::iterator_category tag
		= typename AnyIter::iterator_category()) {
		construct(rng.first, rng.second, tag);
	}
	valvec(const std::pair<const T*, const T*>& rng) {
		construct(rng.first, rng.second - rng.first);
	}
	valvec(const T* first, const T* last) { construct(first, last-first); }
	valvec(const T* first, ptrdiff_t len) { construct(first, len); }
	template<class AnyIter>
	valvec(AnyIter first, AnyIter last
		, typename AnyIter::iterator_category tag
		= typename AnyIter::iterator_category()) 	{
		construct(first, last, tag);
	}
	template<class InputIter>
	valvec(InputIter first, ptrdiff_t count
		, typename InputIter::iterator_category
		= typename InputIter::iterator_category()) 	{
		assert(count >= 0);
		construct(first, count);
	}

	valvec(const valvec& y) {
        assert(this != &y);
		assert(!is_object_overlap(this, &y));
		construct(y.p, y.n);
    }

    valvec& operator=(const valvec& y) {
        if (&y == this)
            return *this;
		assert(!is_object_overlap(this, &y));
        clear();
		construct(y.p, y.n);
        return *this;
    }

#ifdef HSM_HAS_MOVE
    valvec(valvec&& y) {
        assert(this != &y);
		assert(!is_object_overlap(this, &y));
		p = y.p;
		n = y.n;
		c = y.c;
		y.p = NULL;
		y.n = 0;
		y.c = 0;
	}

	valvec& operator=(valvec&& y) {
        assert(this != &y);
        if (&y == this)
            return *this;
		assert(!is_object_overlap(this, &y));
		this->clear();
		new(this)valvec(y);
		return *this;
	}
#endif // HSM_HAS_MOVE

    ~valvec() { clear(); }

    void fill(const T& x) {
        std::fill_n(p, n, x);
    }

    template<class AnyIter>
	typename void_<typename AnyIter::iterator_category>::type
    assign(const std::pair<AnyIter, AnyIter>& rng) {
		assign_aux(rng.first, rng.second, typename AnyIter::iterator_category());
    }
    template<class AnyIter>
	typename void_<typename AnyIter::iterator_category>::type
    assign(AnyIter first, AnyIter last) {
		assign_aux(first, last, typename AnyIter::iterator_category());
    }
    template<class InputIter>
	typename void_<typename InputIter::iterator_category>::type
    assign(InputIter first, ptrdiff_t len) {
		assert(len >= 0);
		erase_all();
        ensure_capacity(len);
		STDEXT_uninitialized_copy_n(first, len, p);
		n = len;
    }
    void assign(const std::pair<const T*, const T*>& rng) {
		assign(rng.first, rng.second);
    }
    void assign(const T* first, const T* last) {
		assign(first, last - first);
    }
    void assign(const T* first, ptrdiff_t len) {
		assert(len >= 0);
		erase_all();
        ensure_capacity(len);
		STDEXT_uninitialized_copy_n(first, len, p);
		n = len;
    }
	template<class Container>
	typename void_<typename Container::const_iterator>::type
	assign(const Container& cont) { assign(cont.begin(), cont.size()); }

    void clear() {
        if (p) {
			STDEXT_destroy_range(p, p + n);
            free(p);
        }
        p = NULL;
        n = c = 0;
    }

	const T* data() const { return p; }
	      T* data()       { return p; }

    bool  empty() const { return 0 == n; }
    size_t size() const { return n; }
    size_t capacity() const { return c; }
	size_t unused() const { return c - n; }

	size_t used_mem_size() const { return sizeof(T) * n; }
	size_t full_mem_size() const { return sizeof(T) * c; }
	size_t free_mem_size() const { return sizeof(T) * (c - n); }

    void reserve(size_t newcap) {
        if (newcap <= c)
            return; // nothing to do
        T* q = (T*)realloc(p, sizeof(T) * newcap);
        if (NULL == q) throw std::bad_alloc();
        p = q;
        c = newcap;
    }

    void ensure_capacity(size_t min_cap) {
        size_t hard_min_cap = 1;
        if (min_cap < hard_min_cap)
            min_cap = hard_min_cap;
        if (min_cap <= c) {
            // nothing to do
            return;
        }
        if (min_cap < 2 * c)
            min_cap = 2 * c;
        T* q = (T*)realloc(p, sizeof(T) * min_cap);
        if (NULL == q) throw std::bad_alloc();
        p = q;
        c = min_cap;
    }

    void shrink_to_fit() {
        assert(n <= c);
        if (n == c)
            return;
        if (n) {
			if (T* q = (T*)realloc(p, sizeof(T) * n)) {
				p = q;
				c = n;
			}
        } else {
            if (p)
                free(p);
            p = NULL;
            c = n = 0;
        }
    }

	// expect this function will reduce memory fragment
	void shrink_to_fit_malloc_free() {
		if (0 == c) return;
		assert(NULL != p);
		if (0 == n) {
			free(p);
			p = NULL;
			n = 0;
			return;
		}
		// malloc new and free old even if n==c
		// because this may trigger the memory compaction
		// of the malloc implementation and reduce memory fragment
		if (T* q = (T*)malloc(sizeof(T) * n)) {
			memcpy(q, p, sizeof(T) * n);
			free(p);
			c = n;
			p = q;
		} else {
			shrink_to_fit();
		}
	}

    void resize(size_t newsize, const T& val = T()) {
        if (newsize == n)
            return; // nothing to do
        if (newsize < n)
			STDEXT_destroy_range(p + newsize, p + n);
        else {
            ensure_capacity(newsize);
            std::uninitialized_fill_n(p+n, newsize-n, val);
        }
        n = newsize;
    }

	void resize_fill(size_t newsize, const T& val = T()) {
        if (newsize <= n) {
			STDEXT_destroy_range(p + newsize, p + n);
			std::fill_n(p, newsize, val);
		}
		else {
			ensure_capacity(newsize);
			std::fill_n(p, n, val);
			std::uninitialized_fill_n(p+n, newsize-n, val);
        }
        n = newsize;
	}

	void erase_all() {
		STDEXT_destroy_range(p, p + n);
		n = 0;
	}

    // client code should pay the risk for performance gain
    void resize_no_init(size_t newsize) {
    //  assert(boost::has_trivial_constructor<T>::value);
        ensure_capacity(newsize);
        n = newsize;
    }
    // client code should pay the risk for performance gain
	void resize0() { n = 0; }

	///  trim [from, end)
	void trim(T* from) {
		assert(from >= p);
		assert(from <= p + n);
		STDEXT_destroy_range(from, p + n);
		n = from - p;
	}

	void insert(const T* pos, const T& x) {
		assert(pos <= p + n);
		assert(pos >= p);
		insert(pos-p, x);
	}

	void insert(size_t pos, const T& x) {
		assert(pos <= n);
		if (pos > n) {
			throw std::out_of_range("valvec::insert");
		}
        if (n == c)
            ensure_capacity(n+1);
	//	for (ptrdiff_t i = n; i > pos; --i) memcpy(p+i, p+i-1, sizeof T);
		memmove(p+pos+1, p+pos, sizeof(T)*(n-pos));
		new(p+pos)T(x);
		++n;
	}

	void push_back() { push_back(T()); }
	void push_back(const T& x) {
		if (n < c) {
			new(p+n)T(x); // copy cons
			++n;
		} else {
			push_back_slow_path(x);
		}
	}
	void push_back_slow_path(const T& x) {
		if (&x >= p && &x < p+n) {
			size_t idx = &x - p;
			ensure_capacity(n+1);
			new(p+n)T(p[idx]); // copy cons
		}
		else {
			ensure_capacity(n+1);
			new(p+n)T(x); // copy cons
		}
		++n;
	}

	void append(const T& x) { push_back(x); } // alias for push_back
	template<class Iterator>
	void append(Iterator first, ptrdiff_t len) {
		assert(len >= 0);
        size_t newsize = n + len;
        ensure_capacity(newsize);
        STDEXT_uninitialized_copy_n(first, len, p+n);
        n = newsize;
	}
	template<class Iterator>
	void append(Iterator first, Iterator last) {
		ptrdiff_t len = std::distance(first, last);
        size_t newsize = n + len;
        ensure_capacity(newsize);
        std::uninitialized_copy(first, last, p+n);
        n = newsize;
	}
	template<class Iterator>
	void append(const std::pair<Iterator, Iterator>& rng) {
		append(rng.first, rng.second);
	}
	template<class Container>
	typename void_<typename Container::const_iterator>::type
	append(const Container& cont) {
		append(cont.begin(), cont.end());
	}
    void unchecked_push_back() { unchecked_push_back(T()); }
    void unchecked_push_back(const T& x) {
		assert(n < c);
        new(p+n)T(x); // copy cons
        ++n;
    }
    void pop_back() {
        if (0 == n)
            throw std::logic_error("valvec::pop_back(), already empty");
        p[n-1].~T();
        --n;
    }
    void unchecked_pop_back() {
        assert(n > 0);
        p[n-1].~T();
        --n;
    }

// use valvec as stack ...
//
    void pop() { pop_back(); }
    void push() { push_back(T()); } // alias for push_back
    void push(const T& x) { push_back(x); } // alias for push_back

	const T& top() const {
        if (0 == n)
            throw std::logic_error("valvec::top() const, valec is empty");
		return p[n-1];
	}
	T& top() {
        if (0 == n)
            throw std::logic_error("valvec::top(), valec is empty");
		return p[n-1];
	}

	T pop_val() {
        if (0 == n)
            throw std::logic_error("valvec::pop_val(), valec is empty");
#ifdef HSM_HAS_MOVE
		T x(std::move(p[n-1]));
#else
		T x(p[n-1]);
        p[n-1].~T();
#endif
		--n;
		return x;
	}
	T unchecked_pop_val() {
		assert(n > 0);
#ifdef HSM_HAS_MOVE
		T x(std::move(p[n-1]));
#else
		T x(p[n-1]);
        p[n-1].~T();
#endif
		--n;
		return x;
	}

    void unchecked_pop() { unchecked_pop_back(); }
    void unchecked_push() { unchecked_push_back(T()); }
    void unchecked_push(const T& x) { unchecked_push_back(x); }

// end use valvec as stack

    const T& operator[](size_t i) const {
        assert(i < n);
        return p[i];
    }

    T& operator[](size_t i) {
        assert(i < n);
        return p[i];
    }

    const T& at(size_t i) const {
        if (i >= n) throw std::out_of_range("valvec::at");
        return p[i];
    }

    T& at(size_t i) {
        if (i >= n) throw std::out_of_range("valvec::at");
        return p[i];
    }

	void set(size_t i, const T& val) {
		assert(i < n);
		p[i] = val;
	}

    const T& front() const {
        assert(n);
        assert(p);
        return p[0];
    }
    T& front() {
        assert(n);
        assert(p);
        return p[0];
    }

    const T& back() const {
        assert(n);
        assert(p);
        return p[n-1];
    }
    T& back() {
        assert(n);
        assert(p);
        return p[n-1];
    }

    T& ende(size_t d) {
        assert(d <= n);
        return p[n-d];
    }
    const T& ende(size_t d) const {
        assert(d <= n);
        return p[n-d];
    }

    void operator+=(const T& x) {
        push_back(x);
    }

    void operator+=(const valvec& y) {
        size_t newsize = n + y.size();
        ensure_capacity(newsize);
        std::uninitialized_copy(y.p, y.p + y.n, p+n);
        n = newsize;
    }

    void swap(valvec& y) {
        std::swap(p, y.p);
        std::swap(n, y.n);
        std::swap(c, y.c);
    }

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103 \
 || defined(_MSC_VER) && _MSC_VER >= 1700
	void push(T&& x) { push_back(std::forward<T>(x)); }
    void push_back(T&& x) {
		if (n < c) {
			new(p+n)T(std::forward<T>(x));
			++n;
		} else {
			push_back_slow_path(std::forward<T>(x));
		}
    }
	void push_back_slow_path(T&& x) {
		assert(n == c);
		if (&x >= p && &x < p+n) {
			size_t idx = &x - p;
			ensure_capacity(n+1);
			new(p+n)T(std::forward<T>(p[idx]));
		}
		else {
			ensure_capacity(n+1);
			new(p+n)T(std::forward<T>(x));
		}
		++n;
	}

	void append(T&& x) { push_back(std::forward<T>(x)); }

    void unchecked_push(T&& x) { unchecked_push_back(std::forward<T>(x)); }
    void unchecked_push_back(T&& x) {
		assert(n < c);
        new(p+n)T(std::forward<T>(x));
        ++n;
    }

#if defined(_MSC_VER) && _MSC_VER <= 1800
// C++: internal compiler error: variadic perfect forwarding
// https://connect.microsoft.com/VisualStudio/feedback/details/806017/c-internal-compiler-error-variadic-perfect-forwarding-to-base-class
// Can not call std::forward multiple times, even in different branch
// this emplace_back is buggy for vec.emplace_back(vec[0]);
	template<class... Args>
	void emplace_back(Args&&... args) {
		ensure_capacity(n + 1);
		new(p+n)T(std::forward<Args>(args)...);
		++n;
	}
#else
	template<class... Args>
	void emplace_back(Args&&... args) {
        if (n < c) {
			new(p+n)T(std::forward<Args>(args)...);
			++n;
		} else {
			emplace_back_slow_path(std::forward<Args>(args)...);
		}
	}
	template<class... Args>
	void emplace_back_slow_path(Args&&... args) {
		T val(std::forward<Args>(args)...);
        ensure_capacity(n+1);
		new(p+n)T(std::move(val));
		++n;
	}
#endif

	explicit valvec(std::initializer_list<T> list) {
		construct(list.begin(), list.size());
	}

#else

	template<class A1>
	void emplace_back(const A1& a1) {
        if (n < c) {
			new(p+n)T(a1);
			++n;
		} else {
			emplace_back_slow_path(a1);
		}
	}
	template<class A1, class A2>
	void emplace_back(const A1& a1, const A2& a2) {
        if (n < c) {
			new(p+n)T(a1, a2);
			++n;
		} else {
			emplace_back_slow_path(a1, a2);
		}
	}
	template<class A1, class A2, class A3>
	void emplace_back(const A1& a1, const A2& a2, const A3& a3) {
        if (n < c) {
			new(p+n)T(a1, a2, a3);
			++n;
		} else {
			emplace_back_slow_path(a1, a2, a3);
		}
	}
	template<class A1, class A2, class A3, class A4>
	void emplace_back(const A1& a1, const A2& a2, const A3& a3, const A4& a4) {
        if (n < c) {
			new(p+n)T(a1, a2, a3, a4);
			++n;
		} else {
			emplace_back_slow_path(a1, a2, a3, a4);
		}
	}

	template<class A1>
	void emplace_back_slow_path(const A1& a1) {
		T val(a1);
        ensure_capacity(n+1);
		new(p+n)T(val);
		++n;
	}
	template<class A1, class A2>
	void emplace_back_slow_path(const A1& a1, const A2& a2) {
		T val(a1, a2);
        ensure_capacity(n+1);
		new(p+n)T(val);
		++n;
	}
	template<class A1, class A2, class A3>
	void emplace_back_slow_path(const A1& a1, const A2& a2, const A3& a3) {
		T val(a1, a2, a3);
        ensure_capacity(n+1);
		new(p+n)T(val);
		++n;
	}
	template<class A1, class A2, class A3, class A4>
	void emplace_back_slow_path(const A1& a1, const A2& a2, const A3& a3, const A4& a4) {
		T val(a1, a2, a3, a4);
        ensure_capacity(n+1);
		new(p+n)T(val);
		++n;
	}

#endif

	size_t erase_i(size_t pos, size_t cnt) {
		assert(cnt <= this->n);
		assert(pos <= this->n);
		assert(pos + cnt <= this->n);
		STDEXT_destroy_range(p + pos, p + pos + cnt);
		memmove(p + pos, p + pos + cnt, sizeof(T) * (n - cnt));
		n -= cnt;
		return pos;
	}

	std::pair<T*, T*> range() { return std::make_pair(p, p+n); }
	std::pair<const T*, const T*> range() const { return std::make_pair(p, p+n); }

	const T& get_2d(size_t colsize, size_t row, size_t col) const {
		size_t idx = row * colsize + col;
		assert(idx < n);
		return p[idx];
	}
	T& get_2d(size_t col_size, size_t row, size_t col) {
		size_t idx = row * col_size + col;
		assert(idx < n);
		return p[idx];
	}

	void risk_set_data(T* Data, size_t Size) {
		p = Data;
		n = Size;
		c = Size;
	}
	void risk_set_data(T* data) { p = data; }
	void risk_set_size(size_t size) { this->n = size; }
	void risk_set_capacity(size_t capa) { this->c = capa; }

	void risk_release_ownership() {
	//	BOOST_STATIC_ASSERT(boost::has_trivial_destructor<T>::value);
		p = NULL;
		n = c = 0;
	}

/*
 * Now serialization valvec<T> has been builtin DataIO
 *
	template<class DataIO>
	friend void DataIO_saveObject(DataIO& dio, const valvec& x) {
		typename DataIO::my_var_uint64_t size(x.n);
		dio << size;
		// complex object has not implemented yet!
		BOOST_STATIC_ASSERT(boost::has_trivial_destructor<T>::value);
		dio.ensureWrite(x.p, sizeof(T)*x.n);
	}

   	template<class DataIO>
	friend void DataIO_loadObject(DataIO& dio, valvec& x) {
		typename DataIO::my_var_uint64_t size;
		dio >> size;
		x.resize_no_init(size.t);
		// complex object has not implemented yet!
		BOOST_STATIC_ASSERT(boost::has_trivial_destructor<T>::value);
		dio.ensureRead(x.p, sizeof(T)*x.n);
	}
*/

    template<class TLess, class TEqual>
    static bool lessThan(const valvec& x, const valvec& y, TLess le, TEqual eq) {
        size_t n = std::min(x.n, y.n);
        for (size_t i = 0; i < n; ++i) {
            if (!eq(x.p[i], y.p[i]))
                return le(x.p[i], y.p[i]);
        }
        return x.n < y.n;
    }
    template<class TEqual>
    static bool equalTo(const valvec& x, const valvec& y, TEqual eq) {
        if (x.n != y.n)
            return false;
        for (size_t i = 0, n = x.n; i < n; ++i) {
            if (!eq(x.p[i], y.p[i]))
                return false;
        }
        return true;
    }
    template<class TLess = std::less<T>, class TEqual = std::equal_to<T> >
    struct less : TLess, TEqual {
        bool operator()(const valvec& x, const valvec& y) const {
            return lessThan<TLess, TEqual>(x, y, *this, *this);
        }
        less() {}
        less(TLess le, TEqual eq) : TLess(le), TEqual(eq) {}
    };
    template<class TEqual = std::equal_to<T> >
    struct equal : TEqual {
        bool operator()(const valvec& x, const valvec& y) const {
            return equalTo<TEqual>(x, y, *this);
        }
        equal() {}
        equal(TEqual eq) : TEqual(eq) {}
    };
};

template<class T>
bool valvec_lessThan(const valvec<T>& x, const valvec<T>& y) {
	return valvec<T>::lessThan(x, y, std::less<T>(), std::equal_to<T>());
}
template<class T>
bool valvec_equalTo(const valvec<T>& x, const valvec<T>& y) {
	return valvec<T>::equalTo(x, y, std::equal_to<T>());
}

namespace std {
	template<class T>
	void swap(valvec<T>& x, valvec<T>& y) { x.swap(y); }
}

/// STL like algorithm with array/RanIt and size_t param

template<class RanIt, class Key>
size_t lower_bound_n(RanIt a, size_t low, size_t upp, const Key& key) {
	size_t i = low, j = upp;
	while (i < j) {
		size_t mid = (i + j) / 2;
		if (a[mid] < key)
			i = mid + 1;
		else
			j = mid;
	}
	return i;
}
template<class RanIt, class Key, class Comp>
size_t lower_bound_n(RanIt a, size_t low, size_t upp, const Key& key, Comp comp) {
	size_t i = low, j = upp;
	while (i < j) {
		size_t mid = (i + j) / 2;
		if (comp(a[mid], key))
			i = mid + 1;
		else
			j = mid;
	}
	return i;
}

template<class RanIt, class Key>
size_t upper_bound_n(RanIt a, size_t low, size_t upp, const Key& key) {
	size_t i = low, j = upp;
	while (i < j) {
		size_t mid = (i + j) / 2;
		if (key < a[mid])
			j = mid;
		else
			i = mid + 1;
	}
	return i;
}

template<class RanIt, class Key, class Comp>
size_t upper_bound_n(RanIt a, size_t low, size_t upp, const Key& key, Comp comp) {
	size_t i = low, j = upp;
	while (i < j) {
		size_t mid = (i + j) / 2;
		if (comp(key, a[mid]))
			j = mid;
		else
			i = mid + 1;
	}
	return i;
}

template<class RanIt, class Key>
std::pair<size_t, size_t>
equal_range_n(RanIt a, size_t low, size_t upp, const Key& key) {
	size_t i = low, j = upp;
	while (i < j) {
		size_t mid = (i + j) / 2;
		if (a[mid] < key)
			i = mid + 1;
		else if (key < a[mid])
			j = mid;
		else
			return std::pair<size_t, size_t>(
				lower_bound_n<RanIt, Key>(a, i, mid, key),
				upper_bound_n<RanIt, Key>(a, mid + 1, upp, key));
	}
	return std::pair<size_t, size_t>(i, i);
}

template<class RanIt, class Key, class Comp>
std::pair<size_t, size_t>
equal_range_n(RanIt a, size_t low, size_t upp, const Key& key, Comp comp) {
	size_t i = low, j = upp;
	while (i < j) {
		size_t mid = (i + j) / 2;
		if (comp(a[mid], key))
			i = mid + 1;
		else if (comp(key, a[mid]))
			j = mid;
		else
			return std::pair<size_t, size_t>(
				lower_bound_n<RanIt, Key, Comp>(a, i, mid, key, comp),
				upper_bound_n<RanIt, Key, Comp>(a, mid+1, j, key, comp));
	}
	return std::pair<size_t, size_t>(i, i);
}

template<class RanIt, class Key>
bool
binary_search_n(RanIt a, size_t low, size_t upp, const Key& key) {
	size_t f = lower_bound_n<RanIt, Key>(a, low, upp, key);
	return f < upp && !(key < a[f]);
}
template<class RanIt, class Key, class Comp>
bool
binary_search_n(RanIt a, size_t low, size_t upp, const Key& key, Comp comp) {
	size_t f = lower_bound_n<RanIt, Key, Comp>(a, low, upp, key, comp);
	return f < upp && !comp(key, a[f]);
}

template<class RanIt>
void sort_n(RanIt a, size_t low, size_t upp) {
	std::sort<RanIt>(a + low, a + upp);
}
template<class RanIt, class Comp>
void sort_n(RanIt a, size_t low, size_t upp, Comp comp) {
	std::sort<RanIt, Comp>(a + low, a + upp, comp);
}

template<class RanIt, class Key>
size_t lower_bound_0(RanIt a, size_t n, const Key& key) {
	return lower_bound_n<RanIt, Key>(a, 0, n, key);
}
template<class RanIt, class Key, class Comp>
size_t lower_bound_0(RanIt a, size_t n, const Key& key, Comp comp) {
	return lower_bound_n<RanIt, Key, Comp>(a, 0, n, key, comp);
}

template<class RanIt, class Key>
size_t upper_bound_0(RanIt a, size_t n, const Key& key) {
	return upper_bound_n<RanIt, Key>(a, 0, n, key);
}
template<class RanIt, class Key, class Comp>
size_t upper_bound_0(RanIt a, size_t n, const Key& key, Comp comp) {
	return upper_bound_n<RanIt, Key, Comp>(a, 0, n, key, comp);
}

template<class RanIt, class Key>
std::pair<size_t, size_t>
equal_range_0(RanIt a, size_t n, const Key& key) {
	return equal_range_n<RanIt, Key>(a, 0, n, key);
}
template<class RanIt, class Key, class Comp>
std::pair<size_t, size_t>
equal_range_0(RanIt a, size_t n, const Key& key, Comp comp) {
	return equal_range_n<RanIt, Key, Comp>(a, 0, n, key, comp);
}

template<class RanIt, class Key>
bool
binary_search_0(RanIt a, size_t n, const Key& key) {
	return binary_search_n<RanIt, Key>(a, 0, n, key);
}
template<class RanIt, class Key, class Comp>
bool
binary_search_0(RanIt a, size_t n, const Key& key, Comp comp) {
	return binary_search_n<RanIt, Key, Comp>(a, 0, n, key, comp);
}

template<class RanIt>
void sort_0(RanIt a, size_t n) {
	sort_n<RanIt>(a, 0, n);
}
template<class RanIt, class Comp>
void sort_0(RanIt a, size_t n, Comp comp) {
	sort_n<RanIt, Comp>(a, 0, n, comp);
}
template<class Container>
void sort_a(Container& a) {
	std::sort(a.begin(), a.end());
}
template<class T, size_t N>
void sort_a(T (&a)[N]) {
	std::sort(a, a+N);
}

template<class RanIt>
void reverse_n(RanIt a, size_t low, size_t upp) {
	std::reverse<RanIt>(a + low, a + upp);
}
template<class RanIt>
void reverse_0(RanIt a, size_t n) {
	std::reverse<RanIt>(a + 0, a + n);
}
template<class Container>
void reverse_a(Container& a) {
	std::reverse(a.begin(), a.end());
}
template<class Container>
void reverse_a(Container& a, size_t low, size_t upp) {
	assert(low <= upp);
	assert(upp <= a.size());
	std::reverse(a.begin() + low, a.begin() + upp);
}

template<class RanIt>
size_t unique_n(RanIt a, size_t low, size_t upp) {
	return std::unique<RanIt>(a + low, a + upp) - a;
}
template<class RanIt>
size_t unique_0(RanIt a, size_t n) {
	return std::unique<RanIt>(a + 0, a + n) - a;
}
template<class Container>
size_t unique_a(Container& a) {
	return std::unique(a.begin(), a.end()) - a.begin();
}
template<class Container>
size_t unique_a(Container& a, size_t low, size_t upp) {
	assert(low <= upp);
	assert(upp <= a.size());
	return std::unique(a.begin() + low, a.begin() + upp) - low - a.begin();
}


#endif

