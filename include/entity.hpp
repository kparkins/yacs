#ifndef YACS_ENTITY_HPP
#define YACS_ENTITY_HPP

#include <functional>

#include "registry.hpp"
#include "types.hpp"

using std::forward;

namespace yacs {

class entity {
 public:
  template <typename T, typename... Args>
  T& add(Args&&... args) {
    return registry->add<T>(id, forward<Args>(args)...);
  }

  template <typename T>
  void remove() {
    registry->destroy<T>(id);
  }

  template <typename T>
  T& get() {
    return registry->get<T>(id);
  }

 protected:
  friend class registry;

  entity(entity_id id, registry* registry) : id(id), registry(registry) {}

  entity_id id;
  registry* registry;
};

}  // namespace yacs

#endif