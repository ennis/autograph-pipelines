#include <algorithm> // std::remove_if
#include "scene.hpp"
#include "rect_transform.hpp"

entity::ptr scene::create_entity() {
	ent_.push_back(std::make_unique<entity>(*this));
	return entity::ptr{ ent_.back().get() };
}

// delete an entity and its transform children
void scene::delete_hierarchy(entity::ptr p)
{
	if (auto rectTransform = p->get_component<rect_transform>()) {
		// hack to avoid reference cycles
		rectTransform->parent = nullptr;
		for (auto&& child : rectTransform->children) 
			delete_hierarchy(child);
	}
	p->destroy();
}

void scene::collect() {
	ent_.erase(std::remove_if(ent_.begin(), ent_.end(), [](auto &up) {
		return up->deleted_ && (up->use_count() == 1);
	}), ent_.end());
}