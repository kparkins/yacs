#ifndef YACS_COMPONENT_H
#define YACS_COMPONENT_H

#include "pool.hpp"

namespace yacs {

template <typename T, typename storage_type = packed_pool<T>>
class component {
 public:
  component() : index(-1), storage(nullptr) {}
  component(storage_type* storage, typename storage_type::index_type index)
      : index(index), storage(storage) {}

  inline T& operator*() { return storage->access(index); }
  inline T* operator->() { return &storage->access(index); }

  inline bool operator==(const component<T>& other) const {
    return storage == other.storage && index == other.index;
  }
  inline bool operator!=(const component<T>& other) const {
    return !(*this == other);
  }

 protected:
  typename storage_type::index_type index;
  storage_type* storage;
};

}  // namespace yacs

#endif
