#include "types.hpp"

yacs::component_id yacs::g_component_id_counter = 0;

yacs::entity_id yacs::get_entity_id(entity_index index,
                                    entity_version version) {
  return (static_cast<entity_id>(index) << 32) | version;
}

yacs::entity_index yacs::get_entity_index(entity_id id) {
  return (id >> 32) & 0xFFFFFFFF;
}

yacs::entity_version yacs::get_entity_version(entity_id id) {
  return id & 0xFFFFFFFF;
}