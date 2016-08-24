#include <vector>
#include <memory>

// slot map implementation by Sean Middleditch
// https://github.com/seanmiddleditch/TinyExperiments/blob/master/SlotMapExample/Main.cpp

typedef long long object_id;

struct slot_object {
	object_id id;
	// other fields
};

template <typename T, size_t ChunkSize>
struct slot_map
{
	static_assert(std::is_base_of<slot_object, T>::value, "T must derive from slot_object");
	using storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;
	std::vector<std::unique_ptr<storage_type[]>> object_table;
	std::vector<int> free_list;

	object_id create_object() {
		if (free_list.empty()) {
			auto chunk = std::make_unique<storage_type[]>(ChunkSize);
			for (int i = chunk_size - 1; i >= 0; --i) {
				// initialize object ids (chunk index + version 0)
				chunk[i].id = object_table.size() * chunk_size + i;
				free_list.push_back(object_table.size() * chunk_size + i);
			}
			object_table.push_back(chunk);
		}

		int free = free_list.back();
		free_list.pop_back();
		return object_table[free / chunk_size][free % chunk_size].id;
	}

	object* get_object(object_id id) {
		object* obj = object_table[(id & 0xFFFFFFFF) / chunk_size] + ((id & 0xFFFFFFFF) % chunk_size);
		return obj->id != id ? nullptr : obj;
	}

	void destroy_object(object_id id) {
		object* obj = get_object(id);
		// increase version id
		obj->id = (obj->id & 0xFFFFFFFF) | (((obj->id >> 32) + 1) << 32);
		free_list.push_back(id & 0xFFFFFFFF);
	}
}