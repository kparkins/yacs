#ifndef YACS_COMPONENT_H
#define YACS_COMPONENT_H

#include "pool.hpp"

namespace yacs {

template <typename T, typename storage_type = packed_pool<T>>
class component {
 public:
  component() : sparse_index(0), storage(nullptr) {}
  component(storage_type* storage,
            typename storage_type::sparse_index_type index)
      : sparse_index(index), storage(storage) {}

  inline T& operator*() { return storage->access(sparse_index); }
  inline T* operator->() { return &storage->access(sparse_index); }

  inline bool operator==(const component<T>& other) const {
    return storage == other.storage && sparse_index == other.sparse_index;
  }
  inline bool operator!=(const component<T>& other) const {
    return !(*this == other);
  }

 protected:
  typename storage_type::sparse_index_type sparse_index;
  storage_type* storage;
};

}  // namespace yacs

#endif
