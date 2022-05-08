#include "pool.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>

#include "data_struct.hpp"

using std::move;

class packed_pool_test : public ::testing::Test {
 protected:
  using value_iterator_type = yacs::packed_pool<data_struct>::value_iterator;
  using const_packed_iterator_type =
      yacs::packed_pool<data_struct>::const_packed_iterator;
  using const_sparse_iterator_type =
      yacs::packed_pool<data_struct>::const_sparse_iterator;

  void SetUp() {
    for (int i = 0; i < 10; i++) {
      pool.construct(i, i, -i);
      expected.construct(i, i, -i);
    }
  }
  yacs::packed_pool<data_struct> empty;
  yacs::packed_pool<data_struct> expected;
  yacs::packed_pool<data_struct> pool;
};

template <typename T>
bool packed_pool_deep_equal(const yacs::packed_pool<T>& left,
                            const yacs::packed_pool<T>& right) {
  if (left.size() != right.size()) {
    return false;
  }
  for (int i = 0; i < left.size(); ++i) {
    auto& left_elem = left.access(i);
    auto& right_elem = right.access(i);
    if (left_elem != right_elem) {
      return false;
    }
  }
  return true;
}

template <typename Iterator>
bool packed_pool_iterator_equal(Iterator& it1, Iterator& it2,
                                const Iterator& end) {
  bool result = true;
  for (; it1 != end; ++it1, ++it2) {
    result &= *it1 == *it2;
  }
  result &= it1 == end;
  result &= it2 == end;
  return result;
}

template <typename T>
void advance_iterator(T& it, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    ++it;
  }
}

TEST_F(packed_pool_test, packed_pool_move_constructor) {
  yacs::packed_pool<data_struct> moved(move(pool));
  ASSERT_EQ(pool.size(), 0);
  ASSERT_EQ(moved.size(), 10);
  ASSERT_FALSE(packed_pool_deep_equal<data_struct>(pool, moved));
  ASSERT_TRUE(packed_pool_deep_equal<data_struct>(expected, moved));
}

TEST_F(packed_pool_test, packed_pool_copy_constructor) {
  yacs::packed_pool<data_struct> copied(pool);
  ASSERT_TRUE(packed_pool_deep_equal<data_struct>(pool, copied));
  ASSERT_TRUE(packed_pool_deep_equal<data_struct>(expected, copied));
}

TEST_F(packed_pool_test, packed_pool_copy_assignment) {
  yacs::packed_pool<data_struct> copied = pool;
  ASSERT_TRUE(packed_pool_deep_equal<data_struct>(pool, copied));
  ASSERT_TRUE(packed_pool_deep_equal<data_struct>(expected, copied));
}

TEST_F(packed_pool_test, packed_pool_move_assignment) {
  yacs::packed_pool<data_struct> moved = std::move(pool);
  ASSERT_EQ(pool.size(), 0);
  ASSERT_EQ(moved.size(), 10);
  ASSERT_FALSE(packed_pool_deep_equal<data_struct>(pool, moved));
  ASSERT_TRUE(packed_pool_deep_equal<data_struct>(expected, moved));
}

TEST_F(packed_pool_test, packed_pool_construct_fn) {
  yacs::packed_pool<data_struct> test;
  for (int i = 0; i < 100; ++i) {
    test.construct(i, 2 * i, -2 * i);
  }
  for (int i = 0; i < 100; ++i) {
    auto& element = test.access(i);
    ASSERT_EQ(*(element.x), 2 * i);
    ASSERT_EQ(element.y, -2 * i);
  }
}

TEST_F(packed_pool_test, packed_pool_destroy_index) {
  ASSERT_EQ(pool.size(), 10);
  for (int i = 0; i < 10; ++i) {
    pool.destroy(i);
    ASSERT_EQ(pool.size(), 9 - i);
  }
  ASSERT_TRUE(packed_pool_deep_equal<data_struct>(pool, empty));
}

TEST_F(packed_pool_test, packed_pool_destroy_all) {
  ASSERT_EQ(pool.size(), 10);
  pool.destroy();
  ASSERT_EQ(pool.size(), 0);
}

TEST_F(packed_pool_test, packed_pool_contains) {
  for (int i = 0; i < 10; ++i) {
    ASSERT_TRUE(pool.contains(i));
  }
  ASSERT_FALSE(pool.contains(11));
  ASSERT_FALSE(pool.contains(static_cast<size_t>(-1)));
}

TEST_F(packed_pool_test, packed_value_access) {
  for (int i = 0; i < pool.size(); ++i) {
    auto& element = pool.access(i);
    *(element.x) *= 2;
    element.y *= 2;
  }
  for (int i = 0; i < pool.size(); ++i) {
    auto& element = pool.access(i);
    auto& reference = expected.access(i);
    ASSERT_EQ(*element.x, (*reference.x) * 2);
    ASSERT_EQ(element.y, reference.y * 2);
  }
}

TEST_F(packed_pool_test, packed_value_array_access) {
  for (int i = 0; i < pool.size(); ++i) {
    pool[i].y *= 2;
    *(pool[i].x) *= 2;
  }
  for (int i = 0; i < pool.size(); ++i) {
    auto& element = pool[i];
    auto& reference = expected[i];
    ASSERT_EQ(*element.x, (*reference.x) * 2);
    ASSERT_EQ(element.y, reference.y * 2);
  }
}

TEST_F(packed_pool_test, packed_pool_immutable_access) {
  for (int i = 0; i < pool.size(); ++i) {
    const auto& element = pool.access(i);
    ASSERT_EQ(element, expected.access(i));
  }
}

TEST_F(packed_pool_test, packed_pool_immutable_array_access) {
  for (int i = 0; i < pool.size(); ++i) {
    const auto& element = pool[i];
    ASSERT_EQ(element, expected[i]);
  }
}

TEST_F(packed_pool_test, packed_pool_size) {
  ASSERT_EQ(pool.size(), 10);
  ASSERT_EQ(expected.size(), 10);
  ASSERT_EQ(empty.size(), 0);
  pool.construct(11, -2, 3);
  ASSERT_EQ(pool.size(), 11);
}

TEST_F(packed_pool_test, packed_pool_reserve_capacity) {
  pool.reserve(10000);
  ASSERT_EQ(pool.capacity(), 10000);
}

TEST_F(packed_pool_test, packed_pool_empty) {
  ASSERT_TRUE(empty.empty());
  ASSERT_FALSE(pool.empty());
}

TEST_F(packed_pool_test, packed_pool_sort) {
  pool.destroy();
  for (int i = 19; i >= 0; --i) {
    pool.construct(i, 2 * i, -2 * i);
  }
  int count = 19;
  for (auto it = pool.begin(); it != pool.end(); ++it) {
    ASSERT_EQ(*it->x, 2 * count);
    ASSERT_EQ(it->y, -2 * count);
    --count;
  }
  pool.sort();
  count = 0;
  for (auto it = pool.begin(); it != pool.end(); ++it) {
    ASSERT_EQ(*it->x, 2 * count);
    ASSERT_EQ(it->y, -2 * count);
    ++count;
  }
}

TEST_F(packed_pool_test, packed_pool_sort_comparator) {
  auto comp = [](const data_struct& lhs, const data_struct& rhs) {
    return lhs.y < rhs.y;
  };
  auto it = pool.begin();
  auto prev = it->y;
  ++it;
  for (; it != pool.end(); ++it) {
    ASSERT_GT(prev, it->y);
  }
  pool.sort(comp);
  it = pool.begin();
  prev = (it++)->y;
  for (; it != pool.end(); ++it) {
    ASSERT_LT(prev, it->y);
  }
}

TEST_F(packed_pool_test, packed_pool_sort_iterator) {
  vector<size_t> indices{5, 1, 3, 7, 2, 0, 9, 8, 4, 6};
  yacs::packed_pool<data_struct> ordered;
  for (auto index : indices) {
    ordered.construct(index, index, index);
  }
  auto it = pool.sparse_begin();
  size_t count = 0;
  for (; it != pool.sparse_end(); ++it) {
    ASSERT_EQ(count++, *it);
  }
  pool.sort(ordered.sparse_begin(), ordered.sparse_end());
  it = pool.sparse_begin();
  count = 0;
  for (; it != pool.sparse_end(); ++it) {
    ASSERT_EQ(indices[count++], *it);
  }
}

TEST_F(packed_pool_test, packed_mutable_iterator_default_constructor) {
  yacs::packed_value_iterator<size_t, data_struct> it1;
  yacs::packed_value_iterator<size_t, data_struct> it2;
  ASSERT_EQ(it1, it2);
}

TEST_F(packed_pool_test, packed_pool_basic_iterator) {
  auto it = pool.begin();
  size_t count = 0;
  for (; it != pool.end(); ++it) {
    count++;
  }
  ASSERT_EQ(pool.size(), count);
}

TEST_F(packed_pool_test, packed_value_iterator_copy_construct) {
  auto it = pool.begin();
  advance_iterator(it, 5);
  auto copy_it(it);
  ASSERT_TRUE(
      packed_pool_iterator_equal<value_iterator_type>(it, copy_it, pool.end()));
}

TEST_F(packed_pool_test, packed_value_iterator_copy_assignment) {
  auto it = pool.begin();
  advance_iterator(it, 3);
  auto copy_it = it;
  ASSERT_TRUE(
      packed_pool_iterator_equal<value_iterator_type>(it, copy_it, pool.end()));
}

TEST_F(packed_pool_test, packed_value_iterator_post_increment) {
  auto post_it = pool.begin();
  auto pre_it = pool.begin();
  advance_iterator(post_it, 1);
  for (; post_it != pool.end();) {
    ASSERT_EQ((post_it++)->y, (++pre_it)->y);
  }
  ASSERT_EQ(post_it, pool.end());
}

TEST_F(packed_pool_test, packed_value_iterator_decrement_last) {
  auto it = pool.end();
  auto& last = pool.access(pool.size() - 1);
  --it;
  ASSERT_EQ(*it, last);
  ASSERT_EQ(*it--, last);
  ASSERT_EQ(*it, pool.access(pool.size() - 2));
}

TEST_F(packed_pool_test, packed_value_iterator_pre_decrement) {
  auto it = pool.begin();
  advance_iterator(it, 3);
  ASSERT_EQ(*((it)->x), 3);
  ASSERT_EQ(*((--it)->x), 2);
  ASSERT_EQ(*((--it)->x), 1);
  ASSERT_EQ(*((--it)->x), 0);
}

TEST_F(packed_pool_test, packed_value_iterator_post_decrement) {
  auto it = pool.begin();
  advance_iterator(it, 2);
  ASSERT_EQ(*((it--)->x), 2);
  ASSERT_EQ(*((it--)->x), 1);
  ASSERT_EQ(*((it--)->x), 0);
}

TEST_F(packed_pool_test, packed_value_iterator_equal) {
  auto it1 = pool.begin();
  auto it2 = pool.begin();
  for (; it2 != pool.end(); ++it1, ++it2) {
    ASSERT_EQ(it1, it2);
  }
}

TEST_F(packed_pool_test, packed_value_iterator_not_equal) {
  auto it1 = pool.begin();
  auto it2 = ++pool.begin();
  for (; it2 != pool.end(); ++it1, ++it2) {
    ASSERT_NE(it1, it2);
  }
}

// const packed iterator
TEST_F(packed_pool_test, const_packed_iterator_default_constructor) {
  yacs::const_packed_iterator<size_t, data_struct> it1;
  yacs::const_packed_iterator<size_t, data_struct> it2;
  ASSERT_EQ(it1, it2);
}

TEST_F(packed_pool_test, packed_pool_packed_iterator) {
  auto it = pool.packed_begin();
  size_t count = 0;
  for (; it != pool.packed_end(); ++it) {
    count++;
  }
  ASSERT_EQ(pool.size(), count);
}

TEST_F(packed_pool_test, const_packed_iterator_copy_construct) {
  auto it = pool.packed_begin();
  advance_iterator(it, 5);
  auto copy_it(it);
  ASSERT_TRUE(packed_pool_iterator_equal<const_packed_iterator_type>(
      it, copy_it, pool.packed_end()));
}

TEST_F(packed_pool_test, const_packed_iterator_copy_assignment) {
  auto it = pool.packed_begin();
  advance_iterator(it, 3);
  auto copy_it = it;
  ASSERT_TRUE(packed_pool_iterator_equal<const_packed_iterator_type>(
      it, copy_it, pool.packed_end()));
}

TEST_F(packed_pool_test, const_packed_iterator_post_increment) {
  auto post_it = pool.packed_begin();
  auto pre_it = pool.packed_begin();
  advance_iterator(post_it, 1);
  for (; post_it != pool.packed_end();) {
    ASSERT_EQ((post_it++)->second.y, (++pre_it)->second.y);
  }
  ASSERT_EQ(post_it, pool.packed_end());
}

TEST_F(packed_pool_test, const_packed_iterator_decrement_last) {
  auto it = pool.packed_end();
  auto& last = pool.access(pool.size() - 1);
  --it;
  ASSERT_EQ(it->second, last);
  ASSERT_EQ((it--)->second, last);
  ASSERT_EQ(it->second, pool.access(pool.size() - 2));
}

TEST_F(packed_pool_test, const_packed_iterator_pre_decrement) {
  auto it = pool.packed_begin();
  advance_iterator(it, 3);
  ASSERT_EQ(*((it)->second.x), 3);
  ASSERT_EQ(*((--it)->second.x), 2);
  ASSERT_EQ(*((--it)->second.x), 1);
  ASSERT_EQ(*((--it)->second.x), 0);
}

TEST_F(packed_pool_test, const_packed_iterator_post_decrement) {
  auto it = pool.packed_begin();
  advance_iterator(it, 2);
  ASSERT_EQ(*((it--)->second.x), 2);
  ASSERT_EQ(*((it--)->second.x), 1);
  ASSERT_EQ(*((it--)->second.x), 0);
}

TEST_F(packed_pool_test, const_packed_iterator_equal) {
  auto it1 = pool.packed_begin();
  auto it2 = pool.packed_begin();
  for (; it2 != pool.packed_end(); ++it1, ++it2) {
    ASSERT_EQ(it1, it2);
  }
}

TEST_F(packed_pool_test, const_packed_iterator_not_equal) {
  auto it1 = pool.packed_begin();
  auto it2 = ++pool.packed_begin();
  for (; it2 != pool.packed_end(); ++it1, ++it2) {
    ASSERT_NE(it1, it2);
  }
}

// const sparse iterator
TEST_F(packed_pool_test, const_sparse_iterator_default_constructor) {
  yacs::const_sparse_iterator<size_t, data_struct> it1;
  yacs::const_sparse_iterator<size_t, data_struct> it2;
  ASSERT_EQ(it1, it2);
}

TEST_F(packed_pool_test, packed_pool_sparse_iterator) {
  auto it = pool.sparse_begin();
  size_t count = 0;
  for (; it != pool.sparse_end(); ++it) {
    count++;
  }
  ASSERT_EQ(pool.size(), count);
}

TEST_F(packed_pool_test, const_sparse_iterator_copy_construct) {
  auto it = pool.sparse_begin();
  advance_iterator(it, 5);
  auto copy_it(it);
  ASSERT_TRUE(packed_pool_iterator_equal<const_sparse_iterator_type>(
      it, copy_it, pool.sparse_end()));
}

TEST_F(packed_pool_test, const_sparse_iterator_copy_assignment) {
  auto it = pool.sparse_begin();
  advance_iterator(it, 3);
  auto copy_it = it;
  ASSERT_TRUE(packed_pool_iterator_equal<const_sparse_iterator_type>(
      it, copy_it, pool.sparse_end()));
}

TEST_F(packed_pool_test, const_sparse_iterator_post_increment) {
  auto post_it = pool.sparse_begin();
  auto pre_it = pool.sparse_begin();
  advance_iterator(post_it, 1);
  for (; post_it != pool.sparse_end();) {
    ASSERT_EQ(*post_it++, *++pre_it);
  }
  ASSERT_EQ(post_it, pool.sparse_end());
}

TEST_F(packed_pool_test, const_sparse_iterator_decrement_last) {
  auto it = pool.sparse_end();
  auto last = pool.size() - 1;
  --it;
  ASSERT_EQ(*it, last);
  ASSERT_EQ(*(it--), last);
  ASSERT_EQ(*it, pool.size() - 2);
}

TEST_F(packed_pool_test, const_sparse_iterator_pre_decrement) {
  auto it = pool.sparse_begin();
  advance_iterator(it, 3);
  ASSERT_EQ(*(it), 3);
  ASSERT_EQ(*(--it), 2);
  ASSERT_EQ(*(--it), 1);
  ASSERT_EQ(*(--it), 0);
}

TEST_F(packed_pool_test, const_sparse_iterator_post_decrement) {
  auto it = pool.sparse_begin();
  advance_iterator(it, 2);
  ASSERT_EQ(*(it--), 2);
  ASSERT_EQ(*(it--), 1);
  ASSERT_EQ(*(it--), 0);
}

TEST_F(packed_pool_test, const_sparse_iterator_equal) {
  auto it1 = pool.sparse_begin();
  auto it2 = pool.sparse_begin();
  for (; it2 != pool.sparse_end(); ++it1, ++it2) {
    ASSERT_EQ(it1, it2);
  }
}

TEST_F(packed_pool_test, const_sparse_iterator_not_equal) {
  auto it1 = pool.sparse_begin();
  auto it2 = ++pool.sparse_begin();
  for (; it2 != pool.sparse_end(); ++it1, ++it2) {
    ASSERT_NE(it1, it2);
  }
}