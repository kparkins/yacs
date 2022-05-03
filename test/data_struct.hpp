#include <memory>

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

  bool operator==(const data_struct& other) const {
    if (x && other.x) {
      return *x == *other.x && y == other.y;
    }
    if (!x && !other.x) {
      return y == other.y;
    }
    return false;
  }

  bool operator!=(const data_struct& other) const { return !(*this == other); }

  friend void swap(data_struct& first, data_struct& second) {
    using std::swap;
    swap(first.x, second.x);
    swap(first.y, second.y);
  }

  int* x;
  int y;
} data_struct;
