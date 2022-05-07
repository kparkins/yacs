#ifndef YACS_ENTITY_HPP
#define YACS_ENTITY_HPP

#include "registry.hpp"
#include "types.hpp"

namespace yacs {

class entity {
 public:
  template <typename T, typename... Args>
  void add(Args&&... args) {}

  template <typename T>
  void remove() {}

 protected:
  friend class registry;
  entity(entity_slot* slot, vector<unique_ptr<pool>>* pools)
      : slot(slot), pools(pools) {}
  entity_slot* slot;
  vector<unique_ptr<pool>>* pools;
};

}  // namespace yacs

#endif