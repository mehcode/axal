#include <cstdio>
#include <cstdlib>

#include "library.h"

// TODO: Fork with #ifdef for WINAPI
#include <dlfcn.h>

axal::Library::Library(const char* filename) : _handle(NULL) {
  // TODO: Fork with #ifdef for WINAPI
  // TODO: Auto apply `.dylib`, `.so`, or `.dll` depending on platform

  _handle = (void*)dlopen(filename, RTLD_LAZY);

  if (!_handle) {
    std::fprintf(stderr, "error: failed to open dynamic library: %s\n", filename);
    std::exit(EXIT_FAILURE);
  }
}

axal::Library::~Library() noexcept {
  if (_handle) {
    // TODO: Fork with #ifdef for WINAPI
    // dlclose(_handle);

    _handle = NULL;
  }
}

void* axal::Library::_get(const char* name) {
    // TODO: Fork with #ifdef for WINAPI
    return dlsym(_handle, name);
}
