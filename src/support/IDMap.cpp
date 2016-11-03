#include <autograph/support/IDMap.h>

namespace ag 
{

uint64_t IDMap::createID() {
  uint64_t id;
  if (freeIDs.empty()) {
    entityIDs.push_back(entityIDs.size() + (1ULL << 32));
    id = entityIDs.back();
  } else {
    id = freeIDs.back();
    freeIDs.pop_back();
  }
  return Entity{ this, id };
}

void IDMap::deleteID(uint64_t eid) {
  // increase generation count
  entityIDs[getEntityIndex(eid)] += 1ULL << 32;
  // add to free list
  freeIDs.push_back(entityIDs[getEntityIndex(eid)]);
}

}