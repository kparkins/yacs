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
      entity_slot* slot = m_free.back();
      m_free.pop_back();
      return entity(&m_entities[slot->index], &m_pools);
    }
    auto index = m_entities.size();
    auto& slot = m_entities.construct(index);
    slot.index = index;
    slot.version = 0;
    slot.mask = 0;
    return entity(&slot, &m_pools);
  }

  void destroy(entity entity) {
    auto& slot = entity.slot;
    auto& mask = slot->mask;
    for (auto i = 0; i < m_pools.size() && i < MAX_COMPONENTS; ++i) {
      if (mask.test(i)) {
        auto& pool = m_pools[i];
        pool->destroy(slot->index);
        mask.reset(i);
      }
    }
    ++slot->version;
    m_free.push_back(slot);
  }

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
        return lhs.index < rhs.index;
      }
      return lhs.mask.to_ullong() < rhs.mask.to_ullong();
    });
  }

 protected:
  vector<entity_slot*> m_free;
  vector<unique_ptr<pool>> m_pools;
  packed_pool<entity_slot> m_entities;
};

}  // namespace yacs

#endif