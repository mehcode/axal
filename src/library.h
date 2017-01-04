#ifndef AXAL_LIBRARY_H
#define AXAL_LIBRARY_H 1

namespace axal {

class Library {
 public:
  Library(const char* filename);

  virtual ~Library() noexcept;

  template <typename T>
  T* get(const char* name) {
    return (T*)_get(name);
  }

 private:
  void* _handle;
  void* _get(const char* name);

};

}  // namespace axal

#endif  // AXAL_LIBRARY_H
