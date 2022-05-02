#include "pool.h"

#include <gtest/gtest.h>

#include <iostream>

typedef struct data_struct {
  data_struct() : x(new int(0)), y(0) {}
  data_struct(int x, int y) : x(new int(x)), y(y) {}
  ~data_struct() {
    if (x) {
      delete x;
    }
  }
  data_struct(const data_struct& other) : x(new int(*other.x)), y(other.y) {}

  data_struct(data_struct&& other) : x(nullptr), y(0) { swap(*this, other); }

  data_struct& operator=(data_struct other) {
    swap(*this, other);
    return *this;
  }

  data_struct& operator=(data_struct&& other) {
    swap(*this, other);
    return *this;
  }

  friend void swap(data_struct& first, data_struct& second) {
    using std::swap;
    swap(first.x, second.x);
    swap(first.y, second.y);
  }

  int* x;
  int y;
} data_struct;

TEST(pool, pool) {
  yacs::dense_pool<data_struct> pool;

  for (int i = 0; i < 10; i++) {
    pool.construct(i, i, -i);
  }

  pool.sort(vector<size_t>{9, 8, 7, 6, 5, 4, 3, 2, 1, 0});
  pool.sort(vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  pool.sort(vector<size_t>{1, 3, 5, 7, 9, 0, 2, 4, 6, 8});
  pool.sort(vector<size_t>{0, 2, 4, 6, 8, 1, 3, 5, 7, 9});

  auto size = pool.size();
  for (int i = 0; i < size; i++) {
    pool.destroy(i);
  }
  ASSERT_EQ(pool.size(), 0);
}