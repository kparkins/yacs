#ifndef YACS_POOL_H
#define YACS_POOL_H

#include <algorithm>
#include <cassert>
#include <functional>
#include <tuple>
#include <vector>

#include "pool_iterator.hpp"

using std::forward;
using std::forward_as_tuple;
using std::function;
using std::get;
using std::pair;
using std::piecewise_construct;
using std::swap;
using std::vector;

namespace yacs {

class pool {
 public:
  virtual ~pool() = default;
};

constexpr size_t POOL_DEFAULT_CAPACITY = 8192;
constexpr size_t POOL_UNALLOCATED_INDEX = static_cast<size_t>(-1);

template <typename T>
class packed_pool {
 public:
  packed_pool();
  packed_pool(packed_pool&& other);
  packed_pool(const packed_pool& other);
  virtual ~packed_pool();

  packed_pool& operator=(const packed_pool& other);
  packed_pool& operator=(packed_pool&& other);

  template <typename... Args>
  T& construct(size_t sparse_index, Args&&... args);

  void destroy(size_t sparse_index);
  void destroy();

  inline bool contains(size_t sparse_index) const;

  inline T& access(size_t sparse_index);
  inline T& operator[](size_t sparse_index);
  inline const T& access(size_t sparse_index) const;
  inline const T& operator[](size_t sparse_index) const;

  inline size_t size() const;
  inline size_t capacity() const;
  inline bool empty() const;
  inline void reserve(size_t n);

  packed_value_iterator<T> begin();
  packed_value_iterator<T> end();

  const_packed_iterator<T> packed_begin() const;
  const_packed_iterator<T> packed_end() const;

  const_sparse_iterator<T> sparse_begin() const;
  const_sparse_iterator<T> sparse_end() const;

  void sort();
  void sort(function<bool(const T&, const T&)> comparator);
  void sort(const vector<size_t>& sparse_order);
  void sort(const_sparse_iterator<T>& begin, const_sparse_iterator<T>& end);

 protected:
  T& internal_access(size_t sparse_index) {
    assert(sparse_index < m_sparse.size());
    assert(m_sparse[sparse_index] != POOL_UNALLOCATED_INDEX);
    return get<1>(m_packed[m_sparse[sparse_index]]);
  }

  const T& internal_access(size_t sparse_index) const {
    assert(sparse_index < m_sparse.size());
    assert(m_sparse[sparse_index] != POOL_UNALLOCATED_INDEX);
    return get<1>(m_packed[m_sparse[sparse_index]]);
  }

  inline void fix_indices() {
    for (size_t i = 0; i < m_packed.size(); ++i) {
      m_sparse[m_packed[i].first] = i;
    }
  }

  vector<pair<size_t, T>> m_packed;
  vector<size_t> m_sparse;
};

template <typename T>
packed_pool<T>::packed_pool() {
  m_packed.reserve(POOL_DEFAULT_CAPACITY);
}

template <typename T>
packed_pool<T>::packed_pool(packed_pool&& other)
    : m_packed(move(other.m_packed)), m_sparse(move(other.m_sparse)) {}

template <typename T>
packed_pool<T>::packed_pool(const packed_pool& other)
    : m_packed(other.m_packed), m_sparse(other.m_sparse) {}

template <typename T>
packed_pool<T>::~packed_pool() {}

template <typename T>
packed_pool<T>& packed_pool<T>::operator=(const packed_pool& other) {
  m_packed = other.m_packed;
  m_sparse = other.m_sparse;
  return *this;
}

template <typename T>
packed_pool<T>& packed_pool<T>::operator=(packed_pool&& other) {
  m_packed = move(other.m_packed);
  m_sparse = move(other.m_sparse);
  return *this;
}

template <typename T>
template <typename... Args>
T& packed_pool<T>::construct(size_t sparse_index, Args&&... args) {
  if (sparse_index >= m_sparse.size()) {
    m_sparse.resize(sparse_index + 1, POOL_UNALLOCATED_INDEX);
  }
  assert(m_sparse[sparse_index] == POOL_UNALLOCATED_INDEX);
  size_t packed_index = m_packed.size();
  m_sparse[sparse_index] = packed_index;
  m_packed.emplace_back(piecewise_construct, forward_as_tuple(sparse_index),
                        forward_as_tuple(forward<Args>(args)...));

  return get<1>(m_packed[packed_index]);
}

template <typename T>
void packed_pool<T>::destroy(size_t sparse_index) {
  assert(sparse_index < m_sparse.size());
  assert(m_sparse[sparse_index] != POOL_UNALLOCATED_INDEX);

  size_t packed_index = m_sparse[sparse_index];
  size_t last_packed_index = m_packed.size() - 1;
  size_t last_sparse_index = get<0>(m_packed[last_packed_index]);

  m_sparse[last_sparse_index] = packed_index;
  m_sparse[sparse_index] = POOL_UNALLOCATED_INDEX;
  if (packed_index != last_packed_index) {
    swap(m_packed[packed_index], m_packed[last_packed_index]);
  }
  m_packed.pop_back();
}

template <typename T>
void packed_pool<T>::destroy() {
  for (int i = 0; i < m_sparse.size(); ++i) {
    if (m_sparse[i] != POOL_UNALLOCATED_INDEX) {
      destroy(i);
    }
  }
}

template <typename T>
inline bool packed_pool<T>::contains(size_t sparse_index) const {
  return sparse_index < m_sparse.size() &&
         m_sparse[sparse_index] != POOL_UNALLOCATED_INDEX;
}

template <typename T>
inline T& packed_pool<T>::access(size_t sparse_index) {
  return internal_access(sparse_index);
}

template <typename T>
inline T& packed_pool<T>::operator[](size_t sparse_index) {
  return internal_access(sparse_index);
}

template <typename T>
inline const T& packed_pool<T>::access(size_t sparse_index) const {
  return internal_access(sparse_index);
}

template <typename T>
inline const T& packed_pool<T>::operator[](size_t sparse_index) const {
  return internal_access(sparse_index);
}

template <typename T>
inline size_t packed_pool<T>::size() const {
  return m_packed.size();
}

template <typename T>
inline size_t packed_pool<T>::capacity() const {
  return m_packed.capacity();
}

template <typename T>
inline bool packed_pool<T>::empty() const {
  return m_packed.empty();
}

template <typename T>
inline void packed_pool<T>::reserve(size_t n) {
  m_packed.reserve(n);
}

template <typename T>
packed_value_iterator<T> packed_pool<T>::begin() {
  return packed_value_iterator<T>(&m_packed);
}

template <typename T>
packed_value_iterator<T> packed_pool<T>::end() {
  return packed_value_iterator<T>(&m_packed, m_packed.size());
}

template <typename T>
const_packed_iterator<T> packed_pool<T>::packed_begin() const {
  return const_packed_iterator<T>(m_packed);
}

template <typename T>
const_packed_iterator<T> packed_pool<T>::packed_end() const {
  return const_packed_iterator<T>(m_packed), m_packed.size();
}

template <typename T>
const_sparse_iterator<T> packed_pool<T>::sparse_begin() const {
  return const_sparse_iterator<T>(&m_packed);
}

template <typename T>
const_sparse_iterator<T> packed_pool<T>::sparse_end() const {
  return const_sparse_iterator<T>(&m_packed, m_packed.size());
}

template <typename T>
void packed_pool<T>::sort() {
  std::sort(m_packed.begin(), m_packed.end(),
            [](auto& left, auto& right) { return left.first < right.first; });
  fix_indices();
}

template <typename T>
void packed_pool<T>::sort(function<bool(const T&, const T&)> comparator) {
  std::sort(m_packed.begin(), m_packed.end(), [&](auto& left, auto& right) {
    return comparator(left.second, right.second);
  });
  fix_indices();
}

template <typename T>
void packed_pool<T>::sort(const vector<size_t>& sparse_order) {
  size_t packed_cursor = 0;
  for (auto sparse_index : sparse_order) {
    if (sparse_index >= m_sparse.size() ||
        m_sparse[sparse_index] == POOL_UNALLOCATED_INDEX) {
      continue;
    }

    size_t sparse_cursor = m_packed[packed_cursor].first;
    if (sparse_index != sparse_cursor) {
      size_t packed_index = m_sparse[sparse_index];
      swap(m_packed[packed_cursor], m_packed[packed_index]);
      swap(m_sparse[sparse_cursor], m_sparse[sparse_index]);
    }
    packed_cursor += 1;
  }
}

template <typename T>
void packed_pool<T>::sort(const_sparse_iterator<T>& begin,
                          const_sparse_iterator<T>& end) {
  // TODO
}

}  // namespace yacs
#endif