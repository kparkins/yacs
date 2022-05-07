#include <gtest/gtest.h>

#include "entity.hpp"
#include "registry.hpp"

typedef struct position {
  int x;
  int y;
} position;

TEST(registry_test, basic_test) {
  yacs::registry registry;
  auto entity = registry.create();
  auto& pos = entity.add<position>();
  pos.x = 1;
  pos.y = 2;
  entity.remove<position>();

  auto& p = entity.get<position>();

  registry.destroy(entity);
}