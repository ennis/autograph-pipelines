#include <algorithm> // std::remove_if
#include "scene.hpp"
#include "rect_transform.hpp"
#include "ui/layout.hpp"
#include "ui/visual.hpp"

entity::ptr scene::create_entity(std::string name) {
	ent_.push_back(std::make_unique<entity>(*this, std::move(name)));
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


void scene::serialize(meta::packer& p)
{
	for (auto&& e : ent_) {
		meta::serialize_dynamic(p, *e);
		if (auto rt = e->get_component<rect_transform>()) {
			meta::serialize_dynamic(p, *rt);
		}
		if (auto vis = e->get_component<ui::visual>()) {
			meta::serialize_dynamic(p, *vis);
		}
		if (auto layout =
			e->get_component<ui::layout_controller>()) {
			meta::serialize_dynamic(p, *layout);
		}
	}
}