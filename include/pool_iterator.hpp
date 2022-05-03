#ifndef YACS_POOL_ITERATOR_H
#define YACS_POOL_ITERATOR_H

#include <utility>
#include <vector>

using std::get;
using std::pair;
using std::vector;

namespace yacs {

template <typename T>
class packed_value_iterator {
 public:
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using difference_type = size_t;
  using iterator_category = std::bidirectional_iterator_tag;

  packed_value_iterator() : index(-1), packed(nullptr) {}

  packed_value_iterator(vector<pair<size_t, T>>* packed, size_t index = 0)
      : index(index), packed(packed) {}

  packed_value_iterator(const packed_value_iterator& other)
      : index(other.index), packed(other.packed) {}

  packed_value_iterator& operator=(packed_value_iterator other) {
    swap(index, other.index);
    swap(packed, other.packed);
    return *this;
  }

  packed_value_iterator& operator++() {
    ++index;
    return *this;
  }

  packed_value_iterator operator++(int) {
    auto copy = *this;
    ++index;
    return copy;
  }

  packed_value_iterator& operator--() {
    --index;
    return *this;
  }

  packed_value_iterator operator--(int) {
    auto copy = *this;
    --(*this);
    return copy;
  }

  bool operator==(const packed_value_iterator& other) const {
    return packed == other.packed && index == other.index;
  }

  bool operator!=(const packed_value_iterator& other) const {
    return !(*this == other);
  }

  T& operator*() const { return (*(packed->data() + index)).second; }
  T* operator->() const { return &(*(packed->data() + index)).second; }

 protected:
  size_t index;
  vector<pair<size_t, T>>* packed;
};

template <typename I, typename T>
class const_packed_iterator {
 public:
  using value_type = pair<I, T>;
  using const_value_type = const pair<I, T>;
  using size_type = typename std::vector<value_type>::size_type;
  using pointer = value_type*;
  using const_pointer = const_value_type*;
  using reference = value_type&;
  using const_reference = const_value_type&;
  using difference_type = size_t;
  using iterator_category = std::bidirectional_iterator_tag;

  const_packed_iterator() : index(-1), packed(nullptr) {}

  const_packed_iterator(vector<value_type>* packed, size_type index = 0)
      : index(index), packed(packed) {}

  const_packed_iterator(const const_packed_iterator& other)
      : index(other.index), packed(other.packed) {}

  const_packed_iterator& operator=(const_packed_iterator other) {
    swap(index, other.index);
    swap(packed, other.packed);
    return *this;
  }

  const_packed_iterator& operator++() {
    ++index;
    return *this;
  }

  const_packed_iterator operator++(int) {
    auto copy = *this;
    ++index;
    return copy;
  }

  const_packed_iterator& operator--() {
    --index;
    return *this;
  }

  const_packed_iterator operator--(int) {
    auto copy = *this;
    --(*this);
    return copy;
  }

  bool operator==(const const_packed_iterator& other) const {
    return packed == other.packed && index == other.index;
  }

  bool operator!=(const const_packed_iterator& other) const {
    return !(*this == other);
  }

  const_reference operator*() const { return *(packed->data() + index); }
  const_pointer operator->() const { return packed->data() + index; }

 protected:
  size_type index;
  const vector<value_type>* packed;
};

template <typename T>
class const_sparse_iterator {
 public:
  const_sparse_iterator() : index(-1), packed(nullptr) {}

  const_sparse_iterator(const vector<pair<size_t, T>>* packed, size_t index = 0)
      : index(index), packed(packed) {}

  const_sparse_iterator(const const_sparse_iterator& other)
      : index(other.index), packed(other.packed) {}

  const_sparse_iterator& operator=(const_sparse_iterator other) {
    swap(index, other.index);
    swap(packed, other.packed);
    return *this;
  }

  const_sparse_iterator& operator++() {
    ++index;
    return *this;
  }

  const_sparse_iterator operator++(int) {
    auto copy = *this;
    ++index;
    return copy;
  }

  const_sparse_iterator& operator--() {
    --index;
    return *this;
  }

  const_sparse_iterator operator--(int) {
    auto copy = *this;
    --(*this);
    return copy;
  }

  bool operator==(const const_sparse_iterator& other) const {
    return packed == other.packed && index == other.index;
  }

  bool operator!=(const const_sparse_iterator& other) const {
    return !(*this == other);
  }

  const size_t& operator*() const { return (*(packed->data() + index)).first; }

  const size_t* operator->() const {
    return &(*(packed->data() + index)).first;
  }

 protected:
  size_t index;
  const vector<pair<size_t, T>>* packed;
};

}  // namespace yacs

#endif