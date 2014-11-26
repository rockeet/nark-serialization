#ifndef __nark_config_h__
#define __nark_config_h__

#if defined(_MSC_VER)

# pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

#  if defined(FEBIRD_CREATE_DLL)
#    pragma warning(disable: 4251)
#    define FEBIRD_DLL_EXPORT __declspec(dllexport)      // creator of dll
#    ifdef _DEBUG
#//	   pragma message("creating nark-d.lib")
#    else
#//	   pragma message("creating nark-r.lib")
#    endif
#  elif defined(FEBIRD_USE_DLL)
#    pragma warning(disable: 4251)
#    define FEBIRD_DLL_EXPORT __declspec(dllimport)      // user of dll
#    ifdef _DEBUG
//#	   pragma comment(lib, "nark-d.lib")
#    else
//#	   pragma comment(lib, "nark-r.lib")
#    endif
#  else
#    define FEBIRD_DLL_EXPORT                            // static lib creator or user
#  endif

#else /* _MSC_VER */

#  define FEBIRD_DLL_EXPORT

#endif /* _MSC_VER */


#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)

#  define nark_likely(x)    __builtin_expect(x, 1)
#  define nark_unlikely(x)  __builtin_expect(x, 0)
#  define nark_no_return    __attribute__((noreturn))
#  define nark_warn_unused_result  __attribute__((warn_unused_result))

#else

#  define nark_no_return
#  define nark_likely(x)    x
#  define nark_unlikely(x)  x
#  define nark_warn_unused_result

#endif

/* The ISO C99 standard specifies that in C++ implementations these
 *    should only be defined if explicitly requested __STDC_CONSTANT_MACROS
 */
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#if defined(__GNUC__) && ( \
	  defined(__LP64__) && (__LP64__ == 1) || \
	  defined(__amd64__) || defined(__amd64) || \
	  defined(__x86_64__) || defined(__x86_64) || \
	  defined(__ia64__) || defined(_IA64) || defined(__IA64__) ) || \
	defined(_MSC_VER) && ( defined(_WIN64) || defined(_M_X64) || defined(_M_IA64) ) || \
	defined(__INTEL_COMPILER) && ( \
	  defined(__ia64) || defined(__itanium__) || \
	  defined(__x86_64) || defined(__x86_64__) ) || \
    defined(__WORD_SIZE) && __WORD_SIZE == 64
  #define FEBIRD_WORD_BITS 64
#else
  #define FEBIRD_WORD_BITS 32
#endif



#endif // __nark_config_h__


