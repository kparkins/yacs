#include <gtest/gtest.h>

#include "component.hpp"
#include "data_struct.hpp"

class component_test : public ::testing::Test {
 protected:
  void SetUp() {
    for (int i = 0; i < 10; ++i) {
      pool.construct(i, 2 * i, 3 * i);
    }
  }

  yacs::packed_pool<data_struct> pool;
};

TEST_F(component_test, component_default_construct_equal) {
  yacs::component<data_struct> component1;
  yacs::component<data_struct> component2;
  ASSERT_EQ(component1, component2);
}

TEST_F(component_test, component_dereference_operator) {
  for (int i = 0; i < 10; ++i) {
    yacs::component<data_struct> component(&pool, i);
    ASSERT_EQ(pool.access(i), *component);
  }
}

TEST_F(component_test, component_arrow_operator) {
  for (int i = 0; i < 10; ++i) {
    yacs::component<data_struct> component(&pool, i);
    auto& element = pool.access(i);
    ASSERT_EQ(element.x, component->x);
    ASSERT_EQ(element.y, component->y);
  }
}

TEST_F(component_test, component_operator_equal) {
  for (int i = 0; i < 10; ++i) {
    yacs::component<data_struct> component1(&pool, i);
    yacs::component<data_struct> component2(&pool, i);
    ASSERT_EQ(component1, component2);
  }
}

TEST_F(component_test, component_operator_not_equal) {
  for (int i = 1; i < 10; ++i) {
    yacs::component<data_struct> component1(&pool, i);
    yacs::component<data_struct> component2(&pool, i - 1);
    ASSERT_NE(component1, component2);
  }
}