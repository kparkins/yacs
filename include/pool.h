#ifndef GAME_POOL_H
#define GAME_POOL_H

#include <algorithm>
#include <cassert>
#include <functional>
#include <tuple>
#include <vector>

using std::forward;
using std::forward_as_tuple;
using std::function;
using std::get;
using std::pair;
using std::piecewise_construct;
using std::swap;
using std::vector;

class pool {
 public:
  virtual ~pool() = default;
};

constexpr size_t POOL_DEFAULT_CAPACITY = 8192;
constexpr size_t POOL_UNALLOCATED_INDEX = static_cast<size_t>(-1);

class dense_pool_iterator {
 public:
};

class dense_pool_const_iterator {
 public:
};

template <typename T>
class dense_pool {
 public:
  dense_pool();
  dense_pool(dense_pool&& other);
  dense_pool(const dense_pool& other);
  virtual ~dense_pool();

  dense_pool& operator=(const dense_pool& other);
  dense_pool& operator=(dense_pool&& other);

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

  dense_pool_iterator begin();
  dense_pool_iterator end();
  dense_pool_const_iterator begin() const;
  dense_pool_const_iterator end() const;

  dense_pool_const_iterator sparse_begin() const;
  dense_pool_const_iterator sparse_end() const;

  void sort();
  void sort(function<bool(const T&, const T&)> comparator);
  void sort(const vector<size_t>& sparse_order);

 protected:
  T& internal_access(size_t sparse_index) {
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
dense_pool<T>::dense_pool() {
  m_packed.reserve(POOL_DEFAULT_CAPACITY);
}

template <typename T>
dense_pool<T>::dense_pool(dense_pool&& other)
    : m_packed(move(other.m_packed)), m_sparse(move(other.m_sparse)) {}

template <typename T>
dense_pool<T>::dense_pool(const dense_pool& other)
    : m_packed(other.m_packed), m_sparse(other.m_sparse) {}

template <typename T>
dense_pool<T>::~dense_pool() {}

template <typename T>
dense_pool<T>& dense_pool<T>::operator=(const dense_pool& other) {
  m_packed = other.m_packed;
  m_sparse = other.m_sparse;
  return *this;
}

template <typename T>
dense_pool<T>& dense_pool<T>::operator=(dense_pool&& other) {
  m_packed = move(other.m_packed);
  m_sparse = move(other.m_sparse);
  return *this;
}

template <typename T>
template <typename... Args>
T& dense_pool<T>::construct(size_t sparse_index, Args&&... args) {
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
void dense_pool<T>::destroy(size_t sparse_index) {
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
void dense_pool<T>::destroy() {
  for (size_t index : m_sparse) {
    if (index != POOL_UNALLOCATED_INDEX) {
      destroy(index);
    }
  }
}

template <typename T>
inline bool dense_pool<T>::contains(size_t sparse_index) const {
  return sparse_index < m_sparse.size() &&
         m_sparse[sparse_index] != POOL_UNALLOCATED_INDEX;
}

template <typename T>
inline T& dense_pool<T>::access(size_t sparse_index) {
  return internal_access(sparse_index);
}

template <typename T>
inline T& dense_pool<T>::operator[](size_t sparse_index) {
  return internal_access(sparse_index);
}

template <typename T>
inline const T& dense_pool<T>::access(size_t sparse_index) const {
  return internal_access(sparse_index);
}

template <typename T>
inline const T& dense_pool<T>::operator[](size_t sparse_index) const {
  return internal_access(sparse_index);
}

template <typename T>
inline size_t dense_pool<T>::size() const {
  return m_packed.size();
}

template <typename T>
inline size_t dense_pool<T>::capacity() const {
  return m_packed.capacity();
}

template <typename T>
inline bool dense_pool<T>::empty() const {
  return m_packed.empty();
}

template <typename T>
inline void dense_pool<T>::reserve(size_t n) {
  m_packed.reserve(n);
}

template <typename T>
dense_pool_iterator dense_pool<T>::begin() {
  return dense_pool_iterator{};
}

template <typename T>
dense_pool_iterator dense_pool<T>::end() {
  return dense_pool_iterator{};
}

template <typename T>
dense_pool_const_iterator dense_pool<T>::begin() const {
  return dense_pool_const_iterator{};
}

template <typename T>
dense_pool_const_iterator dense_pool<T>::end() const {
  return dense_pool_const_iterator{};
}

template <typename T>
dense_pool_const_iterator dense_pool<T>::sparse_begin() const {
  return dense_pool_const_iterator{};
}

template <typename T>
dense_pool_const_iterator dense_pool<T>::sparse_end() const {
  return dense_pool_const_iterator{};
}

template <typename T>
void dense_pool<T>::sort() {
  std::sort(m_packed.begin(), m_packed.end(),
            [](auto& left, auto& right) { return left.first < right.first; });
  fix_indices();
}

template <typename T>
void dense_pool<T>::sort(function<bool(const T&, const T&)> comparator) {
  std::sort(m_packed.begin(), m_packed.end(), [&](auto& left, auto& right) {
    return comparator(left.second, right.second);
  });
  fix_indices();
}

template <typename T>
void dense_pool<T>::sort(const vector<size_t>& sparse_order) {
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

#endif