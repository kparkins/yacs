#ifndef YACS_TYPES_H
#define YACS_TYPES_H

#include <bitset>
#include <cstdint>

using std::bitset;
using std::uint32_t;
using std::uint64_t;

namespace yacs {

typedef uint64_t component_id;
constexpr uint32_t MAX_COMPONENTS = sizeof(component_id);
typedef bitset<MAX_COMPONENTS> component_mask;

typedef uint64_t entity_id;
typedef uint32_t entity_index;
typedef uint32_t entity_version;

typedef struct entity_slot {
  entity_index index;
  entity_version version;
  component_mask mask;
} entity_slot;

extern component_id g_component_id_counter;

template <typename T>
struct component_traits {
  static component_id id() {
    static component_id id = g_component_id_counter++;
    return id;
  }
};

}  // namespace yacs

#endif