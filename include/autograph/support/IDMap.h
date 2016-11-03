#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace ag {

int getIDIndex(uint64_t id) { return static_cast<int>(id & 0xFFFFFFFF); }
int getIDGeneration(uint64_t id) { return static_cast<int>(id >> 32); }

template <typename T> using IDMap = boost::container::flat_map<uint64_t, T>;

class AG_API IDTable {
public:
  ///
  uint64_t createID();
  ///
  void deleteID(uint64_t eid);
  /// return the number of live IDs
  size_t size() const { return IDs.size(); }

  auto &getIDs() { return IDs; }

  bool isValid(uint64_t eid) {
    return (getEntityIndex(eid) < entityIDs.size()) &&
           (getEntityGeneration(eid) ==
            getEntityGeneration(entityIDs[getEntityIndex(eid)]));
  }

private:
  std::vector<uint64_t> entityIDs;
  std::vector<uint64_t> freeIDs;
};

template <typename T> using IDMap = boost::container::flat_map<uint64_t, T>;

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
}
}
