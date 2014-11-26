#ifndef __nark_util_mmap_hpp__
#define __nark_util_mmap_hpp__

#include <stddef.h>
#include "../config.hpp"

namespace nark {

FEBIRD_DLL_EXPORT void  mmap_close(int fd, void* base);
FEBIRD_DLL_EXPORT void* mmap_load(const char* fname, int* fd, size_t* base);

} // namespace nark

#endif // __nark_util_mmap_hpp__

