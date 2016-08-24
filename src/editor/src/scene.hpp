#pragma once
#include <vector> // std::vector
#include "entity.hpp"
#include "observable.hpp"
#include <msgpack.hpp>
#include "meta.hpp"

// a container for entities
class scene {
public:
	// create a new entity in the scene without any components
	entity::ptr create_entity(std::string name);
  // delete an entity and its transform children
  void delete_hierarchy(entity::ptr p);
  // reclaim memory of deleted entities
  void collect();
  // return the number of entities in the scene
  size_t size() const { return ent_.size(); }
  // return a view of the entities
  auto &get_entities() {
	  return ent_;
  }
  // return all entities in the scene that have a component of type T
  template <typename T> auto find(bool include_deleted = false) const {
    /*using namespace ranges;
    // range-v3 is cool, but incredibly slow to compile
    return ent_ | view::remove_if([include_deleted](auto &&u) {
    return (!u->template get_component<T>()) ||
    (!include_deleted && u->deleted_);
    }) |
    view::transform([](auto &&e) { return e.get(); });*/
	  static_assert(std::is_base_of<component_counter_base, T>::value, "stuff");
    return entity_view{*this, T::family()};
  }
  // create a new entity in the scene and add initial components
  template <typename... Components>
  entity::ptr create_entity(std::string name, Components&&... comp) {
	  auto p = create_entity(name);
	  using namespace boost;
	  hana::for_each(std::forward_as_tuple(comp...), [&p](auto &&c) {
		  using CompT = std::decay_t<decltype(c)>;
		  p->add_component<CompT>(std::forward<CompT>(c));
	  });
	  return p;
  }
  // serialize the scene to a msgpack buffer
  void serialize(meta::packer& p);

  // update() is triggered once per frame if the scene is active
  observable<void> update;

private:
  std::vector<std::unique_ptr<entity>> ent_;

  class entity_iterator {
  public:
    entity_iterator(const scene &s, unsigned family, size_t current = 0)
        : s_{s}, family_{family}, cur_{current} {
      skip();
    }

    entity_iterator &operator++() {
      ++cur_;
      skip();
      return *this;
    }

    entity *operator*() { return s_.ent_[cur_].get(); }

    friend inline bool operator==(const scene::entity_iterator &it1,
                                  const scene::entity_iterator &it2) {
      return (it1.family_ == it2.family_) && (it1.cur_ == it2.cur_);
    }

    friend inline bool operator!=(const scene::entity_iterator &it1,
                                  const scene::entity_iterator &it2) {
      return !(it1 == it2);
    }

  private:
    void skip() {
      while (cur_ < s_.size() && s_.ent_[cur_]->comp_[family_] == nullptr)
        ++cur_;
    }

    const scene &s_;
    unsigned family_;
    size_t cur_;
  };

  struct entity_view {
    const scene &s_;
    unsigned family_;
    entity_view(const scene &scene, unsigned family)
        : s_{scene}, family_{family} {}
    entity_iterator begin() const { return entity_iterator{s_, family_, 0}; }
    entity_iterator end() const {
      return entity_iterator{s_, family_, s_.size()};
    }
  };
};
