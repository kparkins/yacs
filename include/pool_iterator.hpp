#ifndef YACS_POOL_ITERATOR_H
#define YACS_POOL_ITERATOR_H

#include <utility>
#include <vector>

using std::get;
using std::pair;
using std::vector;

namespace yacs {

template <typename I, typename T>
class packed_value_iterator {
 public:
  using value_type = T;
  using internal_value_type = pair<I, T>;
  using const_value_type = const T;
  using size_type = typename std::vector<internal_value_type>::size_type;
  using pointer = value_type*;
  using const_pointer = const_value_type*;
  using reference = value_type&;
  using const_reference = const_value_type&;
  using difference_type = typename std::vector<internal_value_type>::size_type;
  using iterator_category = std::bidirectional_iterator_tag;

  packed_value_iterator() : index(-1), packed(nullptr) {}

  packed_value_iterator(vector<internal_value_type>* packed,
                        size_type index = 0)
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

  reference operator*() const { return (*(packed->data() + index)).second; }
  pointer operator->() const { return &(*(packed->data() + index)).second; }

 protected:
  size_type index;
  vector<internal_value_type>* packed;
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

  const_packed_iterator(const vector<value_type>* packed, size_type index = 0)
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

template <typename I, typename T>
class const_sparse_iterator {
 public:
  using value_type = I;
  using internal_value_type = pair<I, T>;
  using const_value_type = const I;
  using size_type = typename std::vector<internal_value_type>::size_type;
  using pointer = value_type*;
  using const_pointer = const_value_type*;
  using reference = value_type&;
  using const_reference = const_value_type&;
  using difference_type = typename std::vector<internal_value_type>::size_type;
  using iterator_category = std::bidirectional_iterator_tag;

  const_sparse_iterator() : index(-1), packed(nullptr) {}

  const_sparse_iterator(const vector<internal_value_type>* packed,
                        size_type index = 0)
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

  const_reference operator*() const {
    return (*(packed->data() + index)).first;
  }

  const_pointer operator->() const {
    return &(*(packed->data() + index)).first;
  }

 protected:
  size_type index;
  const vector<internal_value_type>* packed;
};

}  // namespace yacs

#endif