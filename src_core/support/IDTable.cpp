#include <autograph/support/IDTable.h>

namespace ag {

ID IDTable::createID() {
  ID id;
  if (freeIDs.empty()) {
    liveIDs.push_back(liveIDs.size() + (1ULL << 32));
    id = liveIDs.back();
  } else {
    id = freeIDs.back();
    freeIDs.pop_back();
  }
  return id;
}

void IDTable::deleteID(ID id) {
  auto index = IDIndex(id);
  // increase generation count
  liveIDs[index] += 1ULL << 32;
  // add to free list
  freeIDs.push_back(liveIDs[index]);
}

}