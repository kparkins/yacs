#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "data_struct.hpp"
#include "pool.h"

using std::move;

class dense_pool_test : public ::testing::Test {
 protected:
  void SetUp() {
    for (int i = 0; i < 10; i++) {
      pool.construct(i, i, -i);
      expected.construct(i, i, -i);
    }
  }
  yacs::dense_pool<data_struct> empty;
  yacs::dense_pool<data_struct> expected;
  yacs::dense_pool<data_struct> pool;
};

template <typename T>
bool dense_pool_deep_equal(const yacs::dense_pool<T>& left,
                           const yacs::dense_pool<T>& right) {
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

TEST_F(dense_pool_test, dense_pool_move_constructor) {
  yacs::dense_pool<data_struct> moved(move(pool));
  ASSERT_EQ(pool.size(), 0);
  ASSERT_EQ(moved.size(), 10);
  ASSERT_FALSE(dense_pool_deep_equal<data_struct>(pool, moved));
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(expected, moved));
}

TEST_F(dense_pool_test, dense_pool_copy_constructor) {
  yacs::dense_pool<data_struct> copied(pool);
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(pool, copied));
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(expected, copied));
}

TEST_F(dense_pool_test, dense_pool_copy_assignment) {
  yacs::dense_pool<data_struct> copied = pool;
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(pool, copied));
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(expected, copied));
}

TEST_F(dense_pool_test, dense_pool_move_assignment) {
  yacs::dense_pool<data_struct> moved = std::move(pool);
  ASSERT_EQ(pool.size(), 0);
  ASSERT_EQ(moved.size(), 10);
  ASSERT_FALSE(dense_pool_deep_equal<data_struct>(pool, moved));
  ASSERT_TRUE(dense_pool_deep_equal<data_struct>(expected, moved));
}

TEST_F(dense_pool_test, dense_pool_construct_fn) {
  yacs::dense_pool<data_struct> test;
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

TEST_F(dense_pool_test, dense_pool_mutable_access) {}