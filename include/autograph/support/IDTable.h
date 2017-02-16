#pragma once
#include <array>
#include <autograph/Config.h>
#include <cstdint>
#include <vector>

namespace ag {

using ID = uint64_t;
inline int IDIndex(ID id) { return static_cast<int>(id & 0xFFFFFFFF); }
inline int IDGeneration(ID id) { return static_cast<int>(id >> 32); }

class AG_API IDTable {
public:
  ID createID();
  void deleteID(ID eid);
  /// return the number of live IDs
  size_t size() const { return liveIDs.size(); }
  //auto &getIDs() { return liveIDs; }
  bool isValid(ID id) {
    auto idx = IDIndex(id);
    return (idx < liveIDs.size()) &&
           (IDGeneration(id) == IDGeneration(liveIDs[idx]));
  }

private:
  std::vector<ID> liveIDs;
  std::vector<ID> freeIDs;
};

// template <typename T> using IDMap = boost::container::flat_map<uint64_t, T>;
/*
template <typename T, typename F>
void collectIDMap(IDMap<T> &map, F condition) {
  for (auto it = map.begin(); it != map.end();) {
    if (condition(it)) {
      it = map.erase(it);
    } else {
      it++;
    }
  }
}

template <typename T>
void collectIDMap(IDMap<T> &map, IDTable &liveObjectTable) {
  // bring out your dead
  collectGeneric(
      map, [this](auto it) { return !liveObjectTable.isValid(it->first); });
}*/
}
