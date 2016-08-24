#pragma once
#include <experimental/filesystem>
#include <texture.hpp>
#include <unordered_map>

struct resource_entry
{
	virtual ~resource_entry()
	{}
	std::experimental::filesystem::path path;
	uint64_t last_load_frame;
};

struct image_resource_entry
{
	virtual ~image_resource_entry()
	{}

	glm::ivec2 size;
	ag::image_format format;
	ag::texture tex;
};

class resource
{
public:
private:
	int index;
	int generation;
};

struct image_resource : public resource
{
	glm::ivec2 size() const;
	ag::image_format format() const;
};

struct resource_manager
{
	std::vector<std::unique_ptr<resource_entry>> resources_;
	std::unordered_map<std::experimental::filesystem::path, resource> index_map_;
};


// 1. access elements by index+version
//		cannot use raw pointers: must keep a ref to the pool? 
//		scene.get_entity(id)
// 2. if version tag doesn't match: the object has been deleted
// 3. pointers may not be kept between frames
// 4. (try to keep valid pointers between frames though)
// 5. fast iteration (no 'holes'?)
// 6. compact storage (no overhead)
// 7. object must be self-deletable?
// 8. object creation must not (immediately) invalidate pointers
// 9. object deletion must not (immediately) invalidate pointer
// 10. Destructors must be run

// Entity groups: implement as components
// Ordered lists: implement as intrusive doubly-linked list 
