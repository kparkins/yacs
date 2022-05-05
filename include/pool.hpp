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

template <typename T>
class packed_pool {
 public:
  using sparse_index_type = size_t;
  using packed_value_type = pair<sparse_index_type, T>;
  using size_type = typename std::vector<packed_value_type>::size_type;
  using value_iterator = packed_value_iterator<sparse_index_type, T>;
  using reverse_value_iterator = std::reverse_iterator<value_iterator>;

  using const_packed_iterator = const_packed_iterator<sparse_index_type, T>;
  using const_reverse_packed_iterator =
      std::reverse_iterator<const_packed_iterator>;

  using const_sparse_iterator = const_sparse_iterator<sparse_index_type, T>;
  using const_reverse_sparse_iterator =
      std::reverse_iterator<const_sparse_iterator>;

  static constexpr size_type DEFAULT_CAPACITY = 8192;
  static constexpr size_type UNALLOCATED_INDEX = static_cast<size_type>(-1);

  packed_pool();
  packed_pool(packed_pool&& other);
  packed_pool(const packed_pool& other);
  virtual ~packed_pool();

  packed_pool& operator=(const packed_pool& other);
  packed_pool& operator=(packed_pool&& other);

  template <typename... Args>
  T& construct(sparse_index_type sparse_index, Args&&... args);

  void destroy(sparse_index_type sparse_index);
  void destroy();

  inline bool contains(sparse_index_type sparse_index) const;

  inline T& access(sparse_index_type sparse_index);
  inline T& operator[](sparse_index_type sparse_index);
  inline const T& access(sparse_index_type sparse_index) const;
  inline const T& operator[](sparse_index_type sparse_index) const;

  inline size_type size() const;
  inline size_type capacity() const;
  inline bool empty() const;
  inline void reserve(size_type n);

  value_iterator begin();
  value_iterator end();
  reverse_value_iterator rbegin();
  reverse_value_iterator rend();

  const_packed_iterator packed_begin() const;
  const_packed_iterator packed_end() const;
  const_reverse_packed_iterator rpacked_begin() const;
  const_reverse_packed_iterator rpacked_end() const;

  const_sparse_iterator sparse_begin() const;
  const_sparse_iterator sparse_end() const;
  const_reverse_sparse_iterator rsparse_begin() const;
  const_reverse_sparse_iterator rsparse_end() const;

  void sort();
  void sort(function<bool(const T&, const T&)> comparator);
  void sort(const_sparse_iterator it, const_sparse_iterator end);

 protected:
  T& internal_access(sparse_index_type sparse_index) {
    assert(sparse_index < m_sparse.size());
    assert(m_sparse[sparse_index] != UNALLOCATED_INDEX);
    return get<1>(m_packed[m_sparse[sparse_index]]);
  }

  const T& internal_access(sparse_index_type sparse_index) const {
    assert(sparse_index < m_sparse.size());
    assert(m_sparse[sparse_index] != UNALLOCATED_INDEX);
    return get<1>(m_packed[m_sparse[sparse_index]]);
  }

  inline void fix_indices() {
    for (size_type i = 0; i < m_packed.size(); ++i) {
      m_sparse[m_packed[i].first] = i;
    }
  }

  vector<pair<sparse_index_type, T>> m_packed;
  vector<sparse_index_type> m_sparse;
};

template <typename T>
packed_pool<T>::packed_pool() {
  m_packed.reserve(DEFAULT_CAPACITY);
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
T& packed_pool<T>::construct(sparse_index_type sparse_index, Args&&... args) {
  if (sparse_index >= m_sparse.size()) {
    m_sparse.resize(sparse_index + 1, UNALLOCATED_INDEX);
  }
  assert(m_sparse[sparse_index] == UNALLOCATED_INDEX);
  sparse_index_type packed_index = m_packed.size();
  m_sparse[sparse_index] = packed_index;
  m_packed.emplace_back(piecewise_construct, forward_as_tuple(sparse_index),
                        forward_as_tuple(forward<Args>(args)...));

  return get<1>(m_packed[packed_index]);
}

template <typename T>
void packed_pool<T>::destroy(sparse_index_type sparse_index) {
  assert(sparse_index < m_sparse.size());
  assert(m_sparse[sparse_index] != UNALLOCATED_INDEX);

  sparse_index_type packed_index = m_sparse[sparse_index];
  sparse_index_type last_packed_index = m_packed.size() - 1;
  sparse_index_type last_sparse_index = get<0>(m_packed[last_packed_index]);

  m_sparse[last_sparse_index] = packed_index;
  m_sparse[sparse_index] = UNALLOCATED_INDEX;
  if (packed_index != last_packed_index) {
    swap(m_packed[packed_index], m_packed[last_packed_index]);
  }
  m_packed.pop_back();
}

template <typename T>
void packed_pool<T>::destroy() {
  for (size_type i = 0; i < m_sparse.size(); ++i) {
    if (m_sparse[i] != UNALLOCATED_INDEX) {
      destroy(i);
    }
  }
}

template <typename T>
inline bool packed_pool<T>::contains(sparse_index_type sparse_index) const {
  return sparse_index < m_sparse.size() &&
         m_sparse[sparse_index] != UNALLOCATED_INDEX;
}

template <typename T>
inline T& packed_pool<T>::access(sparse_index_type sparse_index) {
  return internal_access(sparse_index);
}

template <typename T>
inline T& packed_pool<T>::operator[](sparse_index_type sparse_index) {
  return internal_access(sparse_index);
}

template <typename T>
inline const T& packed_pool<T>::access(sparse_index_type sparse_index) const {
  return internal_access(sparse_index);
}

template <typename T>
inline const T& packed_pool<T>::operator[](
    sparse_index_type sparse_index) const {
  return internal_access(sparse_index);
}

template <typename T>
inline typename packed_pool<T>::size_type packed_pool<T>::size() const {
  return m_packed.size();
}

template <typename T>
inline typename packed_pool<T>::size_type packed_pool<T>::capacity() const {
  return m_packed.capacity();
}

template <typename T>
inline bool packed_pool<T>::empty() const {
  return m_packed.empty();
}

template <typename T>
inline void packed_pool<T>::reserve(size_type n) {
  m_packed.reserve(n);
}

template <typename T>
typename packed_pool<T>::value_iterator packed_pool<T>::begin() {
  return value_iterator(&m_packed);
}

template <typename T>
typename packed_pool<T>::value_iterator packed_pool<T>::end() {
  return value_iterator(&m_packed, m_packed.size());
}

template <typename T>
typename packed_pool<T>::reverse_value_iterator packed_pool<T>::rbegin() {
  return std::make_reverse_iterator(end());
}

template <typename T>
typename packed_pool<T>::reverse_value_iterator packed_pool<T>::rend() {
  return std::make_reverse_iterator(begin());
}

template <typename T>
typename packed_pool<T>::const_packed_iterator packed_pool<T>::packed_begin()
    const {
  return const_packed_iterator(&m_packed);
}

template <typename T>
typename packed_pool<T>::const_packed_iterator packed_pool<T>::packed_end()
    const {
  return const_packed_iterator(&m_packed, m_packed.size());
}

template <typename T>
typename packed_pool<T>::const_reverse_packed_iterator
packed_pool<T>::rpacked_begin() const {
  return std::make_reverse_iterator(packed_end());
}

template <typename T>
typename packed_pool<T>::const_reverse_packed_iterator
packed_pool<T>::rpacked_end() const {
  return std::make_reverse_iterator(packed_begin());
}

template <typename T>
typename packed_pool<T>::const_sparse_iterator packed_pool<T>::sparse_begin()
    const {
  return const_sparse_iterator(&m_packed);
}

template <typename T>
typename packed_pool<T>::const_sparse_iterator packed_pool<T>::sparse_end()
    const {
  return const_sparse_iterator(&m_packed, m_packed.size());
}

template <typename T>
typename packed_pool<T>::const_reverse_sparse_iterator
packed_pool<T>::rsparse_begin() const {
  return std::make_reverse_iterator(sparse_begin());
}

template <typename T>
typename packed_pool<T>::const_reverse_sparse_iterator
packed_pool<T>::rsparse_end() const {
  return std::make_reverse_iterator(sparse_end());
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
void packed_pool<T>::sort(const_sparse_iterator it, const_sparse_iterator end) {
  size_type packed_cursor = 0;
  for (; it != end; ++it) {
    auto sparse_index = *it;
    if (sparse_index >= m_sparse.size() ||
        m_sparse[sparse_index] == UNALLOCATED_INDEX) {
      continue;
    }

    size_type sparse_cursor = m_packed[packed_cursor].first;
    if (sparse_index != sparse_cursor) {
      size_type packed_index = m_sparse[sparse_index];
      swap(m_packed[packed_cursor], m_packed[packed_index]);
      swap(m_sparse[sparse_cursor], m_sparse[sparse_index]);
    }
    packed_cursor += 1;
  }
}

}  // namespace yacs
#endif