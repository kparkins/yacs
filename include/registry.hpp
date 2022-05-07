#ifndef YACS_REGISTRY_H
#define YACS_REGISTRY_H

#include <bitset>
#include <cstdint>
#include <memory>
#include <vector>

#include "pool.hpp"
#include "types.hpp"

using std::unique_ptr;
using std::vector;

namespace yacs {

class entity;

class registry {
 public:
  template <typename T>
  using storage_type = yacs::packed_pool<T>;

  registry() {}
  ~registry() {
    for (auto p : m_pools) {
      delete p;
    }
  }

  registry(registry&& other) {
    swap(m_entities, other.m_entities);
    swap(m_free, other.m_free);
    swap(m_pools, other.m_pools);
  }

  registry& operator=(registry&& other) {
    swap(m_entities, other.m_entities);
    swap(m_free, other.m_free);
    swap(m_pools, other.m_pools);
    return *this;
  }

  entity create();
  entity get(entity_id id);

  void destroy(entity_id id);
  void destroy(entity entity);

  template <typename T>
  void destroy(entity_id id) {
    auto component_index = component_traits<T>::id();
    if (component_index >= m_pools.size()) {
      return;
    }
    storage_type<T>* pool =
        static_cast<storage_type<T>*>(m_pools[component_index]);
    pool->destroy(get_entity_index(id));
  }

  template <typename T, typename... Args>
  T& add(entity_id id, Args&&... args) {
    auto component_index = component_traits<T>::id();
    if (component_index >= m_pools.size()) {
      m_pools.emplace_back(new packed_pool<T>());
    }
    storage_type<T>* pool =
        static_cast<storage_type<T>*>(m_pools[component_index]);
    return pool->construct(get_entity_index(id), forward<Args>(args)...);
  }

  template <typename T>
  T& get(entity_id id) {
    auto component_index = component_traits<T>::id();
    assert(component_index < m_pools.size());
    storage_type<T>* pool =
        static_cast<storage_type<T>*>(m_pools[component_index]);
    return pool->access(get_entity_index(id));
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
  registry(const registry& other) = delete;
  registry& operator=(const registry& other) = delete;

  vector<entity_slot*> m_free;
  vector<pool*> m_pools;
  packed_pool<entity_slot> m_entities;
};

}  // namespace yacs

#endif