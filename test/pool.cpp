#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "data_struct.hpp"
#include "pool.hpp"

using std::move;

class dense_pool_test : public ::testing::Test {
 protected:
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
bool dense_pool_deep_equal(const yacs::packed_pool<T>& left,
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

template <typename T>
bool dense_pool_iterator_equal(yacs::packed_value_iterator<T>& it1,
                               yacs::packed_value_iterator<T>& it2,
                               const yacs::packed_value_iterator<T>& end) {
  bool result = true;
  for (; it1 != end; ++it1, ++it2) {
    result &= *it1 == *it2;
  }
  result &= it1 == end;
  result &= it2 == end;
  return result;
}

template <typename T>
void move_iterator(T& it, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    ++it;
  }
}

TEST_F(dense_pool_test, dense_pool_move_constructor) {
  yacs::packed_pool<data_struct> moved(move(pool));
  ASSERT_EQ(pool.size(), 0);
  ASSERT_EQ(moved.size(), 10);
  ASSERT_FALSE(dense_pool_deep_equal<data_struct>(pool, moved));
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(expected, moved));
}

TEST_F(dense_pool_test, dense_pool_copy_constructor) {
  yacs::packed_pool<data_struct> copied(pool);
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(pool, copied));
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(expected, copied));
}

TEST_F(dense_pool_test, dense_pool_copy_assignment) {
  yacs::packed_pool<data_struct> copied = pool;
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(pool, copied));
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(expected, copied));
}

TEST_F(dense_pool_test, dense_pool_move_assignment) {
  yacs::packed_pool<data_struct> moved = std::move(pool);
  ASSERT_EQ(pool.size(), 0);
  ASSERT_EQ(moved.size(), 10);
  ASSERT_FALSE(dense_pool_deep_equal<data_struct>(pool, moved));
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(expected, moved));
}

TEST_F(dense_pool_test, dense_pool_construct_fn) {
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

TEST_F(dense_pool_test, dense_pool_destroy_index) {
  ASSERT_EQ(pool.size(), 10);
  for (int i = 0; i < 10; ++i) {
    pool.destroy(i);
    ASSERT_EQ(pool.size(), 9 - i);
  }
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(pool, empty));
}

TEST_F(dense_pool_test, dense_pool_destroy_all) {
  ASSERT_EQ(pool.size(), 10);
  pool.destroy();
  ASSERT_EQ(pool.size(), 0);
}

TEST_F(dense_pool_test, dense_pool_contains) {
  for (int i = 0; i < 10; ++i) {
    ASSERT_TRUE(pool.contains(i));
  }
  ASSERT_FALSE(pool.contains(11));
  ASSERT_FALSE(pool.contains(-1));
}

TEST_F(dense_pool_test, dense_pool_mutable_access) {
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

TEST_F(dense_pool_test, dense_pool_mutable_array_access) {
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

TEST_F(dense_pool_test, dense_pool_immutable_access) {
  for (int i = 0; i < pool.size(); ++i) {
    const auto& element = pool.access(i);
    ASSERT_EQ(element, expected.access(i));
  }
}

TEST_F(dense_pool_test, dense_pool_immutable_array_access) {
  for (int i = 0; i < pool.size(); ++i) {
    const auto& element = pool[i];
    ASSERT_EQ(element, expected[i]);
  }
}

TEST_F(dense_pool_test, dense_pool_size) {
  ASSERT_EQ(pool.size(), 10);
  ASSERT_EQ(expected.size(), 10);
  ASSERT_EQ(empty.size(), 0);
  pool.construct(11, -2, 3);
  ASSERT_EQ(pool.size(), 11);
}

TEST_F(dense_pool_test, dense_pool_reserve_capacity) {
  pool.reserve(10000);
  ASSERT_EQ(pool.capacity(), 10000);
}

TEST_F(dense_pool_test, dense_pool_empty) {
  ASSERT_TRUE(empty.empty());
  ASSERT_FALSE(pool.empty());
}

TEST_F(dense_pool_test, dense_pool_basic_iterator) {
  auto it = pool.begin();
  size_t count = 0;
  for (; it != pool.end(); ++it) {
    count++;
  }
  ASSERT_EQ(pool.size(), count);
}

TEST_F(dense_pool_test, dense_pool_mutable_iterator_copy_construct) {
  auto it = pool.begin();
  move_iterator(it, 5);
  auto copy_it(it);
  ASSERT_TRUE(dense_pool_iterator_equal<data_struct>(it, copy_it, pool.end()));
}

TEST_F(dense_pool_test, dense_pool_mutable_iterator_copy_assignment) {
  auto it = pool.begin();
  move_iterator(it, 3);
  auto copy_it = it;
  ASSERT_TRUE(dense_pool_iterator_equal<data_struct>(it, copy_it, pool.end()));
}

TEST_F(dense_pool_test, dense_pool_mutable_iterator_post_increment) {
  auto post_it = pool.begin();
  auto pre_it = pool.begin();
  move_iterator(post_it, 1);
  for (; post_it != pool.end();) {
    ASSERT_EQ((post_it++)->y, (++pre_it)->y);
  }
  ASSERT_EQ(post_it, pool.end());
}