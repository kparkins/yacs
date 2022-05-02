#include <gtest/gtest.h>

#include <iostream>

#include "Pool.h"

typedef struct data_struct {
  data_struct() = default;
  data_struct(int x, int y) : x(new int(x)), y(y) {}
  ~data_struct() {
    std::cout << "In destructor " << x << "  " << y << std::endl;
    if (x) {
      std::cout << "Deleting " << *x << std::endl;
      delete x;
    }
  }
  data_struct(const data_struct& other) {
    x = new int(*other.x);
    y = other.y;
  }
  data_struct(data_struct&& other) {
    x = other.x;
    other.x = nullptr;
    y = other.y;
  }
  data_struct& operator=(const data_struct& other) {
    if (&other != this) {
      delete x;
      x = new int(*other.x);
      y = other.y;
    }
    return *this;
  }
  data_struct& operator=(data_struct&& other) {
    if (this != &other) {
      delete x;
      x = other.x;
      other.x = nullptr;
      y = other.y;
    }
    return *this;
  }

  int* x;
  int y;
} data_struct;

TEST(MAIN, test) {
  dense_pool<data_struct> pool;

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