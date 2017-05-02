#pragma once
#include <autograph/Core/Support/IDTable.h>
#include <array>
#include <cstdint>
#include <vector>

namespace ag {

// indirect index map for sparse associative arrays
/*template <typename T>
class SparseMap
{
public:
	 
	T& operator[](ID id) {
		auto idx = IDIndex(id);
		if (idx >= indexTable.size()) {
			indexTable.resize(idx+1);
			//indexTable[idx] = 
		}
		auto trueidx = indexTable[idx];
		if (!trueidx) {
			// create new
			values.push_back(T{});
			trueidx = values.size();
			indexTable[idx] = trueidx;
		}
		else {
			return 
		}
	}

private:
	std::vector<int> indexTable;	// one-based indices, zero means unmapped
	std::vector<T> values;
};*/

//template <typename T> using IDMap = boost::container::flat_map<uint64_t, T>;

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
