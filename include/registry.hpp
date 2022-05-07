#ifndef YACS_REGISTRY_H
#define YACS_REGISTRY_H

#include <bitset>
#include <cstdint>
#include <memory>
#include <vector>

#include "entity.hpp"
#include "pool.hpp"
#include "types.hpp"

using std::unique_ptr;
using std::vector;

namespace yacs {

class registry {
 public:
  registry() {}

  entity create() {
    if (m_free.size() > 0) {
    }
  }
  void destroy() {}

  entity get(entity_index index) {
    return entity(&m_entities[index], &m_pools);
  }

  void view() {}

  void sort(function<bool(const entity_slot&, const entity_slot&)> comparator) {
    m_entities.sort(comparator);
  }

  void sort() {
    m_entities.sort([](const entity_slot& lhs, const entity_slot& rhs) {
      if (lhs.mask == rhs.mask) {
        return lhs.id < rhs.id;
      }
      return lhs.mask.to_ullong() < rhs.mask.to_ullong();
    });
  }

 protected:
  vector<entity_index> m_free;
  vector<unique_ptr<pool>> m_pools;
  packed_pool<entity_slot> m_entities;
};

}  // namespace yacs

#endif