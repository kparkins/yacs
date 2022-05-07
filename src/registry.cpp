#include "registry.hpp"
#include "entity.hpp"

yacs::entity yacs::registry::create() {
  if (m_free.size() > 0) {
    entity_slot* slot = m_free.back();
    m_free.pop_back();
    return entity(get_entity_id(slot->index, slot->version), this);
  }
  auto index = m_entities.size();
  auto& slot = m_entities.construct(index);
  slot.index = index;
  slot.version = 0;
  slot.mask.reset();
  return entity(get_entity_id(slot.index, slot.version), this);
}

void yacs::registry::destroy(entity_id id) {
  auto& slot = m_entities[get_entity_index(id)];
  auto& mask = slot.mask;
  for (auto i = 0; i < m_pools.size() && i < MAX_COMPONENTS; ++i) {
    if (mask.test(i)) {
      auto& pool = m_pools[i];
      pool->destroy(slot.index);
      mask.reset(i);
    }
  }
  ++slot.version;
  m_free.push_back(&slot);
}

void yacs::registry::destroy(entity entity) {
  destroy(entity.id);
}

yacs::entity yacs::registry::get(entity_id id) {
  return entity(id, this);
}